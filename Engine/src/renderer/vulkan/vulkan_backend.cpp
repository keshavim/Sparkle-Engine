//
// Created by overlord on 7/1/25.
//
#include "spa_pch.h"
#include "vulkan_backend.h"


VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_types,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data);


namespace Sparkle {
    bool VulkanBackend::init() {
        // Setup Vulkan instance.
        VkApplicationInfo app_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
        app_info.apiVersion = VK_API_VERSION_1_3;
        app_info.pApplicationName = Application::GetName();
        app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.pEngineName = "Sparkle Engine";
        app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);

        std::vector<const char*> extension_names =  load_extensions();
        SPA_ASSERT(!extension_names.empty());

        VkInstanceCreateInfo create_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
        create_info.pApplicationInfo = &app_info;
        create_info.enabledExtensionCount = static_cast<u32>(extension_names.size());
        create_info.ppEnabledExtensionNames = extension_names.data();

        setup_validation_layers(create_info);

        VkResult res = vkCreateInstance(&create_info, m_allocator, &m_instance);
        VK_CHECK(res);

        res = setup_debugger(m_instance, m_allocator, m_debug_messenger);
        VK_CHECK(res);
        SPA_LOG_DEBUG("Vulkan debug messenger created.");

        bool result = SDL_Vulkan_CreateSurface(Application::GetWindow(), m_instance, m_allocator, &m_surface);
        SPA_ASSERT(result == true);
        SPA_LOG_DEBUG("Vulkan surface created");


        result = VulkanDevice::Create(&m_device, m_instance, m_surface);
        SPA_ASSERT(result == true);
        SPA_LOG_DEBUG("Vulkan physical and logical device created");


        SPA_LOG_INFO("Vulkan renderer initialized successfully.");

        return true;
    }

    void VulkanBackend::shutdown() {
        SPA_LOG_DEBUG("Destroying Vulkan debugger...");
        if (m_debug_messenger) {
            auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
                vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT"));
            if (func) {
                func(m_instance, m_debug_messenger, m_allocator);
            }
            m_debug_messenger = VK_NULL_HANDLE;
        }

        SPA_LOG_DEBUG("Destroying Vulkan surface...");
        if (m_surface) {
            vkDestroySurfaceKHR(m_instance, m_surface, m_allocator);
            m_surface = VK_NULL_HANDLE;
        }

        SPA_LOG_DEBUG("Destroying Vulkan devices...");
        m_device.Cleanup();

        SPA_LOG_DEBUG("Destroying Vulkan instance...");
        if (m_instance) {
            vkDestroyInstance(m_instance, m_allocator);
            m_instance = VK_NULL_HANDLE;
        }
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

