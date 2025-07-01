//
// Created by overlord on 7/1/25.
//
#include "vulkan_backend.h"

#include "vulkan_utils.inl"
#include "core/logger.h"

namespace Sparkle {
    bool VulkanBackend::init() {
        SPA_LOG_TRACE("hello vulkan");

        // TODO: custom allocator.
        m_allocator = 0;

        // Setup Vulkan instance.
        VkApplicationInfo app_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
        app_info.apiVersion = VK_API_VERSION_1_2;
        app_info.pApplicationName = Application::GetName();
        app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.pEngineName = "Sparkle Engine";
        app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);

        VkInstanceCreateInfo create_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
        create_info.pApplicationInfo = &app_info;
        create_info.enabledExtensionCount = 0;
        create_info.ppEnabledExtensionNames = 0;
        create_info.enabledLayerCount = 0;
        create_info.ppEnabledLayerNames = 0;

        VkResult result = vkCreateInstance(&create_info, m_allocator, &m_instance);


        if (result != VK_SUCCESS) {
            SPA_LOG_ERROR("vkCreateInstance failed with result: {}", vk_result_to_string(result));
            return false;
        }
        SPA_LOG_INFO("Vulkan renderer initialized successfully.");

        return true;
    }

    void VulkanBackend::shutdown() {
    }

    void VulkanBackend::resize(uint32_t width, uint32_t height) {
    }

    bool VulkanBackend::begin_frame() {
        return true;
    }

    bool VulkanBackend::end_frame() {
        return true;
    }
}
