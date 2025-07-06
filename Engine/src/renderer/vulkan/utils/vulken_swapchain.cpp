//
// Created by overlord on 7/6/25.
//
#include "spa_pch.h"
#include "../vulkan_utils.h"

VulkanSwapchain::~VulkanSwapchain() {
    // Destruction is manual
}

void VulkanSwapchain::cleanup(VkDevice device) {
    for (auto fb : m_framebuffers) {
        vkDestroyFramebuffer(device, fb, nullptr);
    }
    m_framebuffers.clear();

    m_image_views.cleanup(device);
    m_render_pass.cleanup(device);

    if (m_swapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(device, m_swapchain, nullptr);
        m_swapchain = VK_NULL_HANDLE;
    }
}

VkResult VulkanSwapchain::recreate(VulkanDevice& device, VkSurfaceKHR surface, uint32_t width, uint32_t height) {
    cleanup(device.get_logical_device());
    return create(device, surface, width, height);
}

VkSurfaceFormatKHR VulkanSwapchain::choose_surface_format(const std::vector<VkSurfaceFormatKHR>& formats) {
    for (const auto& format : formats) {
        if (format.format == VK_FORMAT_B8G8R8A8_UNORM &&
            format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return format;
    }
    return formats[0];
}

VkPresentModeKHR VulkanSwapchain::choose_present_mode(const std::vector<VkPresentModeKHR>& modes) {
    for (const auto& mode : modes) {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
            return mode;
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanSwapchain::choose_extent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height) {
    if (capabilities.currentExtent.width != UINT32_MAX)
        return capabilities.currentExtent;

    VkExtent2D actual = { width, height };
    actual.width = std::max(capabilities.minImageCount, std::min(capabilities.maxImageCount, actual.width));
    actual.height = std::max(capabilities.minImageCount, std::min(capabilities.maxImageCount, actual.height));
    return actual;
}

VkResult VulkanSwapchain::create(VulkanDevice& device, VkSurfaceKHR surface, uint32_t width, uint32_t height) {
    VkPhysicalDevice phys = device.get_physical_device();
    VkDevice logical = device.get_logical_device();

    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(phys, surface, &capabilities);

    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(phys, surface, &format_count, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(phys, surface, &format_count, formats.data());

    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(phys, surface, &present_mode_count, nullptr);
    std::vector<VkPresentModeKHR> present_modes(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(phys, surface, &present_mode_count, present_modes.data());

    VkSurfaceFormatKHR surface_format = choose_surface_format(formats);
    VkPresentModeKHR present_mode = choose_present_mode(present_modes);
    VkExtent2D extent = choose_extent(capabilities, width, height);

    uint32_t image_count = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && image_count > capabilities.maxImageCount)
        image_count = capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR create_info = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
    create_info.surface = surface;
    create_info.minImageCount = image_count;
    create_info.imageFormat = surface_format.format;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queue_indices[] = {
        device.get_graphics_queue_family(),
        device.get_present_queue_family()
    };

    if (queue_indices[0] != queue_indices[1]) {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = queue_indices;
    } else {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    create_info.preTransform = capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = VK_NULL_HANDLE;

    VkResult result = vkCreateSwapchainKHR(logical, &create_info, nullptr, &m_swapchain);
    if (result != VK_SUCCESS) return result;

    vkGetSwapchainImagesKHR(logical, m_swapchain, &image_count, nullptr);
    m_images.resize(image_count);
    vkGetSwapchainImagesKHR(logical, m_swapchain, &image_count, m_images.data());

    m_image_format = surface_format.format;
    m_extent = extent;

    result = m_image_views.create(device, m_images, m_image_format, m_extent);
    if (result != VK_SUCCESS) return result;

    result = m_render_pass.create(logical, m_image_format, m_image_views.get_depth_format());
    if (result != VK_SUCCESS) return result;

    // === Create framebuffers ===
    const auto& color_views = m_image_views.get_color_views();
    VkImageView depth_view = m_image_views.get_depth_view();

    m_framebuffers.resize(color_views.size());
    for (size_t i = 0; i < color_views.size(); ++i) {
        VkImageView attachments[] = {
            color_views[i],
            depth_view
        };

        VkFramebufferCreateInfo fb_info = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
        fb_info.renderPass = m_render_pass.get();
        fb_info.attachmentCount = 2;
        fb_info.pAttachments = attachments;
        fb_info.width = extent.width;
        fb_info.height = extent.height;
        fb_info.layers = 1;

        vkCreateFramebuffer(logical, &fb_info, nullptr, &m_framebuffers[i]);
    }

    return VK_SUCCESS;
}

void VulkanSwapchain::test() const {
    std::cout << "[Swapchain Test]\n";
    std::cout << "- Swapchain format: " << m_image_format << "\n";
    std::cout << "- Extent: " << m_extent.width << " x " << m_extent.height << "\n";
    std::cout << "- Images: " << m_images.size() << "\n";
    std::cout << "- Framebuffers: " << m_framebuffers.size() << "\n";
    std::cout << "- Color Views: " << m_image_views.get_color_views().size() << "\n";
    std::cout << "- Depth Format: " << m_image_views.get_depth_format() << "\n";
}
