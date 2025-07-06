//
// Created by overlord on 7/6/25.
//
#include "../vulkan_utils.h"

VulkanImageViews::~VulkanImageViews() {
    // Cleanup is manual
}

void VulkanImageViews::cleanup(VkDevice device) {
    for (auto view : m_color_views) {
        vkDestroyImageView(device, view, nullptr);
    }
    m_color_views.clear();

    if (m_depth_view) {
        vkDestroyImageView(device, m_depth_view, nullptr);
        m_depth_view = VK_NULL_HANDLE;
    }
    if (m_depth_image) {
        vkDestroyImage(device, m_depth_image, nullptr);
        m_depth_image = VK_NULL_HANDLE;
    }
    if (m_depth_memory) {
        vkFreeMemory(device, m_depth_memory, nullptr);
        m_depth_memory = VK_NULL_HANDLE;
    }
}

VkFormat VulkanImageViews::choose_depth_format(VkPhysicalDevice phys) {
    const VkFormat candidates[] = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT
    };
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(phys, format, &props);
        if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            return format;
        }
    }
    assert(false && "No suitable depth format found.");
}

VkResult VulkanImageViews::create(VulkanDevice& device, const std::vector<VkImage>& images, VkFormat color_format, VkExtent2D extent) {
    cleanup(device.get_logical_device()); // Ensure clean state

    m_color_views.resize(images.size());

    // === Create color image views ===
    for (size_t i = 0; i < images.size(); ++i) {
        VkImageViewCreateInfo view_info = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
        view_info.image = images[i];
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format = color_format;
        view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;

        VkResult res = vkCreateImageView(device.get_logical_device(), &view_info, nullptr, &m_color_views[i]);
        if (res != VK_SUCCESS) return res;
    }

    // === Create depth image + memory ===
    m_depth_format = choose_depth_format(device.get_physical_device());

    VkImageCreateInfo image_info = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.extent = { extent.width, extent.height, 1 };
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.format = m_depth_format;
    image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult res = vkCreateImage(device.get_logical_device(), &image_info, nullptr, &m_depth_image);
    if (res != VK_SUCCESS) return res;

    VkMemoryRequirements mem_reqs;
    vkGetImageMemoryRequirements(device.get_logical_device(), m_depth_image, &mem_reqs);

    VkMemoryAllocateInfo alloc_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    alloc_info.allocationSize = mem_reqs.size;

    VkPhysicalDeviceMemoryProperties mem_props;
    vkGetPhysicalDeviceMemoryProperties(device.get_physical_device(), &mem_props);
    for (uint32_t i = 0; i < mem_props.memoryTypeCount; ++i) {
        if ((mem_reqs.memoryTypeBits & (1 << i)) &&
            (mem_props.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
            alloc_info.memoryTypeIndex = i;
            break;
        }
    }

    res = vkAllocateMemory(device.get_logical_device(), &alloc_info, nullptr, &m_depth_memory);
    if (res != VK_SUCCESS) return res;

    vkBindImageMemory(device.get_logical_device(), m_depth_image, m_depth_memory, 0);

    VkImageViewCreateInfo view_info = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    view_info.image = m_depth_image;
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format = m_depth_format;
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;

    return vkCreateImageView(device.get_logical_device(), &view_info, nullptr, &m_depth_view);
}
