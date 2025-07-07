//
// Created by overlord on 7/6/25.
//
#include "spa_pch.h"
#include "../vulkan_utils.h"

VulkanSyncObjects::~VulkanSyncObjects() {
    // Make sure cleanup is called explicitly before destruction
}

VkResult VulkanSyncObjects::create(VkDevice device, uint32_t max_frames_in_flight) {
    m_image_available_semaphores.resize(max_frames_in_flight);
    m_render_finished_semaphores.resize(max_frames_in_flight);
    m_in_flight_fences.resize(max_frames_in_flight);

    VkSemaphoreCreateInfo semaphore_info = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
    VkFenceCreateInfo fence_info = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT; // So we don't wait indefinitely on first frame

    for (uint32_t i = 0; i < max_frames_in_flight; ++i) {
        if (vkCreateSemaphore(device, &semaphore_info, nullptr, &m_image_available_semaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device, &semaphore_info, nullptr, &m_render_finished_semaphores[i]) != VK_SUCCESS ||
            vkCreateFence(device, &fence_info, nullptr, &m_in_flight_fences[i]) != VK_SUCCESS) {
            return VK_ERROR_INITIALIZATION_FAILED;
        }
    }

    return VK_SUCCESS;
}

void VulkanSyncObjects::cleanup(VkDevice device) {
    for (size_t i = 0; i < m_image_available_semaphores.size(); ++i) {
        if (m_image_available_semaphores[i] != VK_NULL_HANDLE) {
            vkDestroySemaphore(device, m_image_available_semaphores[i], nullptr);
            m_image_available_semaphores[i] = VK_NULL_HANDLE;
        }
        if (m_render_finished_semaphores[i] != VK_NULL_HANDLE) {
            vkDestroySemaphore(device, m_render_finished_semaphores[i], nullptr);
            m_render_finished_semaphores[i] = VK_NULL_HANDLE;
        }
        if (m_in_flight_fences[i] != VK_NULL_HANDLE) {
            vkDestroyFence(device, m_in_flight_fences[i], nullptr);
            m_in_flight_fences[i] = VK_NULL_HANDLE;
        }
    }

    m_image_available_semaphores.clear();
    m_render_finished_semaphores.clear();
    m_in_flight_fences.clear();
}

void VulkanSyncObjects::test() const {
    std::cout << "=== VulkanSyncObjects Test ===\n";
    std::cout << "Number of frames in flight: " << m_in_flight_fences.size() << "\n";
    for (size_t i = 0; i < m_in_flight_fences.size(); ++i) {
        std::cout << "Frame " << i << ":\n";
        std::cout << "  Image Available Semaphore: " << m_image_available_semaphores[i] << "\n";
        std::cout << "  Render Finished Semaphore: " << m_render_finished_semaphores[i] << "\n";
        std::cout << "  In Flight Fence: " << m_in_flight_fences[i] << "\n";
    }
}
