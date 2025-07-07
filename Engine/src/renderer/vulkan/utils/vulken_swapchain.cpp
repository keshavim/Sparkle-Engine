#include "spa_pch.h"
#include "../vulkan_utils.h"

VulkanSwapchain::~VulkanSwapchain() {
    // cleanup must be called manually before destruction
}

VkResult VulkanSwapchain::create(VulkanDevice& device, VkSurfaceKHR surface, uint32_t width, uint32_t height) {
    VkDevice vk_device = device.get_logical_device();
    VkPhysicalDevice phys_device = device.get_physical_device();

    // 1. Query surface capabilities
    VkSurfaceCapabilitiesKHR surface_capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(phys_device, surface, &surface_capabilities);

    // 2. Query supported formats
    uint32_t format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(phys_device, surface, &format_count, nullptr);
    assert(format_count > 0);
    std::vector<VkSurfaceFormatKHR> surface_formats(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(phys_device, surface, &format_count, surface_formats.data());

    // 3. Query present modes
    uint32_t present_mode_count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(phys_device, surface, &present_mode_count, nullptr);
    assert(present_mode_count > 0);
    std::vector<VkPresentModeKHR> present_modes(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(phys_device, surface, &present_mode_count, present_modes.data());

    // Choose best surface format, present mode, extent
    VkSurfaceFormatKHR surface_format = choose_surface_format(surface_formats);
    VkPresentModeKHR present_mode = choose_present_mode(present_modes);
    VkExtent2D extent = choose_extent(surface_capabilities, width, height);

    m_format = surface_format.format;
    m_extent = extent;

    // 4. Setup swapchain create info
    uint32_t image_count = surface_capabilities.minImageCount + 1;
    if (surface_capabilities.maxImageCount > 0 && image_count > surface_capabilities.maxImageCount)
        image_count = surface_capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR swapchain_info{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
    swapchain_info.surface = surface;
    swapchain_info.minImageCount = image_count;
    swapchain_info.imageFormat = surface_format.format;
    swapchain_info.imageColorSpace = surface_format.colorSpace;
    swapchain_info.imageExtent = extent;
    swapchain_info.imageArrayLayers = 1;
    swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // Determine how to handle swapchain images across multiple queue families
    uint32_t queue_families[] = { device.get_graphics_queue_family(), device.get_present_queue_family() };
    if (queue_families[0] != queue_families[1]) {
        swapchain_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_info.queueFamilyIndexCount = 2;
        swapchain_info.pQueueFamilyIndices = queue_families;
    } else {
        swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchain_info.queueFamilyIndexCount = 0;
        swapchain_info.pQueueFamilyIndices = nullptr;
    }

    swapchain_info.preTransform = surface_capabilities.currentTransform;
    swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_info.presentMode = present_mode;
    swapchain_info.clipped = VK_TRUE;
    swapchain_info.oldSwapchain = m_swapchain;

    // 5. Create swapchain
    VkResult result = vkCreateSwapchainKHR(vk_device, &swapchain_info, nullptr, &m_swapchain);
    if (result != VK_SUCCESS) {
        std::cerr << "Failed to create swapchain!\n";
        return result;
    }

    // If we have an old swapchain, destroy it (recreation case)
    if (swapchain_info.oldSwapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(vk_device, swapchain_info.oldSwapchain, nullptr);
    }

    // 6. Get swapchain images
    uint32_t swapchain_image_count = 0;
    vkGetSwapchainImagesKHR(vk_device, m_swapchain, &swapchain_image_count, nullptr);
    m_images.resize(swapchain_image_count);
    vkGetSwapchainImagesKHR(vk_device, m_swapchain, &swapchain_image_count, m_images.data());

    // 7. Create image views for swapchain images and create depth image + view
    result = m_image_views.create(device, m_images, m_format, m_extent);
    if (result != VK_SUCCESS) {
        std::cerr << "Failed to create image views!\n";
        return result;
    }

    // 8. Create render pass with chosen formats
    result = m_render_pass.create(vk_device, m_format, m_image_views.get_depth_format());
    if (result != VK_SUCCESS) {
        std::cerr << "Failed to create render pass!\n";
        return result;
    }

    // 9. Create framebuffers for each swapchain image + depth image
    result = m_framebuffers.create(vk_device,
                                  m_image_views.get_color_views(),
                                  m_image_views.get_depth_view(),
                                  m_render_pass.get(),
                                  m_extent);
    if (result != VK_SUCCESS) {
        std::cerr << "Failed to create framebuffers!\n";
        return result;
    }

    // 10. Create command pool for graphics queue family
    result = m_command_pool.create(vk_device, device.get_graphics_queue_family());
    if (result != VK_SUCCESS) {
        std::cerr << "Failed to create command pool!\n";
        return result;
    }

    // 11. Allocate command buffers (one per framebuffer)
    result = m_command_pool.allocate_buffers(vk_device, m_framebuffers.get_all().size());
    if (result != VK_SUCCESS) {
        std::cerr << "Failed to allocate command buffers!\n";
        return result;
    }

    // 12. Record command buffers (render pass begin/end with clear values)
    VkClearValue clear_color{};
    clear_color.color = { {1.0f, 0.0f, 0.0f, 1.0f} };

    VkClearValue clear_depth{};
    clear_depth.depthStencil = {1.0f, 0};

    m_recorder.record_all(m_command_pool.get_buffers(),
                          m_render_pass.get(),
                          m_framebuffers.get_all(),
                          m_extent,
                          clear_color,
                          clear_depth);

    return VK_SUCCESS;
}

void VulkanSwapchain::recreate(VulkanDevice& device, VkSurfaceKHR surface, uint32_t width, uint32_t height) {
    cleanup(device.get_logical_device());
    create(device, surface, width, height);
}

void VulkanSwapchain::cleanup(VkDevice device) {
    m_command_pool.cleanup(device);
    m_framebuffers.cleanup(device);
    m_render_pass.cleanup(device);
    m_image_views.cleanup(device);

    if (m_swapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(device, m_swapchain, nullptr);
        m_swapchain = VK_NULL_HANDLE;
    }
}

void VulkanSwapchain::test() const {
    std::cout << "=== VulkanSwapchain Test ===\n";
    std::cout << "Format: " << m_format << "\n";
    std::cout << "Extent: " << m_extent.width << " x " << m_extent.height << "\n";
    std::cout << "Swapchain images: " << m_images.size() << "\n";

    m_image_views.test();
    m_render_pass.get() ? std::cout << "Render pass created\n" : std::cout << "No render pass\n";
    m_framebuffers.test();
    m_command_pool.test();
}

// Helper picks best format (prefers BGRA8 + SRGB color space)
VkSurfaceFormatKHR VulkanSwapchain::choose_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats) {
    for (const auto& format : available_formats) {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return format;
    }
    return available_formats[0];
}

// Helper picks best present mode (prefers MAILBOX, else FIFO)
VkPresentModeKHR VulkanSwapchain::choose_present_mode(const std::vector<VkPresentModeKHR>& available_modes) {
    for (const auto& mode : available_modes) {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
            return mode;
    }
    return VK_PRESENT_MODE_FIFO_KHR; // guaranteed to be supported
}

// Helper chooses extent based on surface capabilities and window size
VkExtent2D VulkanSwapchain::choose_extent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height) {
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        VkExtent2D actual_extent = { width, height };
        actual_extent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actual_extent.width));
        actual_extent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actual_extent.height));
        return actual_extent;
    }



}