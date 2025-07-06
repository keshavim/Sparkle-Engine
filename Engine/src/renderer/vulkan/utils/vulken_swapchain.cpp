//
// Created by overlord on 7/6/25.
//
#include "spa_pch.h"
#include "../vulkan_utils.h"

VulkanSwapchain::~VulkanSwapchain() {
    // Cleanup is manual
}

void VulkanSwapchain::cleanup(VkDevice device) {
    for (auto view : m_image_views) {
        vkDestroyImageView(device, view, nullptr);
    }
    m_image_views.clear();

    if (m_swapchain) {
        vkDestroySwapchainKHR(device, m_swapchain, nullptr);
        m_swapchain = VK_NULL_HANDLE;
    }
}

VkResult VulkanSwapchain::create(VulkanDevice& device, VkSurfaceKHR surface, uint32_t width, uint32_t height) {
    VkPhysicalDevice phys = device.get_physical_device();
    VkDevice logical = device.get_logical_device();

    // Query surface capabilities
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(phys, surface, &capabilities);

    // Query surface formats
    uint32_t format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(phys, surface, &format_count, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(phys, surface, &format_count, formats.data());

    // Query present modes
    uint32_t mode_count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(phys, surface, &mode_count, nullptr);
    std::vector<VkPresentModeKHR> present_modes(mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(phys, surface, &mode_count, present_modes.data());

    // Choose best format
    VkSurfaceFormatKHR chosen_format = formats[0];
    for (const auto& format : formats) {
        if (format.format == VK_FORMAT_B8G8R8A8_UNORM &&
            format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            chosen_format = format;
            break;
        }
    }

    // Choose present mode (prefer MAILBOX)
    VkPresentModeKHR chosen_mode = VK_PRESENT_MODE_FIFO_KHR;
    for (const auto& mode : present_modes) {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            chosen_mode = mode;
            break;
        }
    }

    // Choose extent
    VkExtent2D extent;
    if (capabilities.currentExtent.width != UINT32_MAX) {
        extent = capabilities.currentExtent;
    } else {
        extent = { width, height };
        extent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, extent.width));
        extent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, extent.height));
    }

    uint32_t image_count = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && image_count > capabilities.maxImageCount) {
        image_count = capabilities.maxImageCount;
    }

    // Create the swapchain
    VkSwapchainCreateInfoKHR create_info = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    create_info.surface = surface;
    create_info.minImageCount = image_count;
    create_info.imageFormat = chosen_format.format;
    create_info.imageColorSpace = chosen_format.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queue_indices[] = {
        device.get_graphics_queue_family(),
        device.get_present_queue_family()
    };

    if (device.get_graphics_queue_family() != device.get_present_queue_family()) {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = queue_indices;
    } else {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    create_info.preTransform = capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = chosen_mode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = VK_NULL_HANDLE;

    VkResult result = vkCreateSwapchainKHR(logical, &create_info, nullptr, &m_swapchain);
    if (result != VK_SUCCESS) return result;

    // Get swapchain images
    uint32_t actual_image_count = 0;
    vkGetSwapchainImagesKHR(logical, m_swapchain, &actual_image_count, nullptr);
    m_images.resize(actual_image_count);
    vkGetSwapchainImagesKHR(logical, m_swapchain, &actual_image_count, m_images.data());

    m_image_format = chosen_format.format;
    m_extent = extent;

    // Create image views
    m_image_views.resize(m_images.size());
    for (size_t i = 0; i < m_images.size(); i++) {
        VkImageViewCreateInfo view_info = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
        view_info.image = m_images[i];
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format = m_image_format;
        view_info.components = {
            VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY
        };
        view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;

        result = vkCreateImageView(logical, &view_info, nullptr, &m_image_views[i]);
        if (result != VK_SUCCESS) return result;
    }

    return VK_SUCCESS;
}

void VulkanSwapchain::test() const {
    std::cout << "Swapchain Format: " << m_image_format << "\n";
    std::cout << "Swapchain Extent: " << m_extent.width << " x " << m_extent.height << "\n";
    std::cout << "Swapchain Image Count: " << m_images.size() << "\n";
}
