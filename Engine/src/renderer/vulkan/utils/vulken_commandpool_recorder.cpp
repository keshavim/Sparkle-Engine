//
// Created by overlord on 7/6/25.
//
#include "spa_pch.h"
#include "../vulkan_utils.h"


void VulkanCommandRecorder::record_all(const std::vector<VkCommandBuffer>& command_buffers,
                                       VkRenderPass render_pass,
                                       const std::vector<VkFramebuffer>& framebuffers,
                                       VkExtent2D extent,
                                       VkClearValue clear_color,
                                       VkClearValue clear_depth) {
    for (size_t i = 0; i < command_buffers.size(); ++i) {
        VkCommandBuffer cmd = command_buffers[i];

        // Begin command buffer
        VkCommandBufferBeginInfo begin_info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        vkBeginCommandBuffer(cmd, &begin_info);

        // Set clear values
        VkClearValue clears[2] = { clear_color, clear_depth };

        VkRenderPassBeginInfo rp_info = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
        rp_info.renderPass = render_pass;
        rp_info.framebuffer = framebuffers[i];
        rp_info.renderArea.offset = { 0, 0 };
        rp_info.renderArea.extent = extent;
        rp_info.clearValueCount = 2;
        rp_info.pClearValues = clears;

        // Begin render pass
        vkCmdBeginRenderPass(cmd, &rp_info, VK_SUBPASS_CONTENTS_INLINE);

        // (This is where future drawing will go)

        // End render pass
        vkCmdEndRenderPass(cmd);

        // End command buffer
        VkResult res = vkEndCommandBuffer(cmd);
        if (res != VK_SUCCESS) {
            std::cerr << "Failed to record command buffer #" << i << "\n";
        }
    }
}
