//
// Created by overlord on 7/6/25.
//
#include "spa_pch.h"
#include "../vulkan_utils.h"

VulkanFramebufferManager::~VulkanFramebufferManager() {
    // Manual cleanup required
}

void VulkanFramebufferManager::cleanup(VkDevice device) {
    for (VkFramebuffer fb : m_framebuffers) {
        vkDestroyFramebuffer(device, fb, nullptr);
    }
    m_framebuffers.clear();
}

VkResult VulkanFramebufferManager::create(VkDevice device,
                                          const std::vector<VkImageView>& color_views,
                                          VkImageView depth_view,
                                          VkRenderPass render_pass,
                                          VkExtent2D extent) {
    cleanup(device);  // Ensure no leftovers

    m_framebuffers.resize(color_views.size());

    for (size_t i = 0; i < color_views.size(); ++i) {
        VkImageView attachments[] = {
            color_views[i],
            depth_view
        };

        VkFramebufferCreateInfo fb_info = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
        fb_info.renderPass = render_pass;
        fb_info.attachmentCount = 2;
        fb_info.pAttachments = attachments;
        fb_info.width = extent.width;
        fb_info.height = extent.height;
        fb_info.layers = 1;

        VkResult res = vkCreateFramebuffer(device, &fb_info, nullptr, &m_framebuffers[i]);
        if (res != VK_SUCCESS) return res;
    }

    return VK_SUCCESS;
}

void VulkanFramebufferManager::test() const {
    std::cout << "[FramebufferManager Test]\n";
    std::cout << "- Framebuffer count: " << m_framebuffers.size() << "\n";
    for (size_t i = 0; i < m_framebuffers.size(); ++i) {
        std::cout << "  [" << i << "] Framebuffer handle: " << m_framebuffers[i] << "\n";
    }
}
