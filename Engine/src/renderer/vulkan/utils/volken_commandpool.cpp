//
// Created by overlord on 7/6/25.
//
#include "spa_pch.h"
#include "../vulkan_utils.h"

VulkanCommandPool::~VulkanCommandPool() {
    // Must call cleanup manually
}

VkResult VulkanCommandPool::create(VkDevice device, uint32_t queue_family_index) {
    VkCommandPoolCreateInfo pool_info = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
    pool_info.queueFamilyIndex = queue_family_index;
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Allow command buffer reuse

    return vkCreateCommandPool(device, &pool_info, nullptr, &m_pool);
}

VkResult VulkanCommandPool::allocate_buffers(VkDevice device, uint32_t count) {
    m_command_buffers.resize(count);

    VkCommandBufferAllocateInfo alloc_info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
    alloc_info.commandPool = m_pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = count;

    return vkAllocateCommandBuffers(device, &alloc_info, m_command_buffers.data());
}

void VulkanCommandPool::cleanup(VkDevice device) {
    if (!m_command_buffers.empty()) {
        vkFreeCommandBuffers(device, m_pool,
                             static_cast<uint32_t>(m_command_buffers.size()),
                             m_command_buffers.data());
        m_command_buffers.clear();
    }

    if (m_pool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(device, m_pool, nullptr);
        m_pool = VK_NULL_HANDLE;
    }
}

void VulkanCommandPool::test() const {
    std::cout << "[CommandPool Test]\n";
    std::cout << "- Command pool handle: " << m_pool << "\n";
    std::cout << "- Command buffer count: " << m_command_buffers.size() << "\n";
    for (size_t i = 0; i < m_command_buffers.size(); ++i) {
        std::cout << "  [" << i << "] Buffer: " << m_command_buffers[i] << "\n";
    }
}
