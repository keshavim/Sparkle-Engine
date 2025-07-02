//
// Created by overlord on 7/1/25.
//
#include "vulkan_backend.h"

#include "vulkan_utils.h"
#include "core/logger.h"
#include "SDL3/SDL_vulkan.h"


VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_types,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data);


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

        //loading extentions
        Uint32 extension_count = 0;

        // Get the extension list from SDL
        const char * const * extensions = SDL_Vulkan_GetInstanceExtensions(&extension_count);
        if (!extensions) {
            SPA_LOG_ERROR("SDL_Vulkan_GetInstanceExtensions failed: {}", SDL_GetError());
            return false;
        }
        std::vector<const char*> extension_names(extensions, extensions + extension_count);
#ifdef SPA_DEBUG
        extension_names.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        extension_names.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        SPA_LOG_DEBUG("Vulkan_Extensions");
        for (auto extension_name : extension_names)
            SPA_LOG_DEBUG("{}", extension_name);
#endif


        VkInstanceCreateInfo create_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
        create_info.pApplicationInfo = &app_info;
        create_info.enabledExtensionCount = static_cast<u32>(extension_names.size());
        create_info.ppEnabledExtensionNames = extension_names.data();

        setup_validation_layers(create_info);

        VkResult res = vkCreateInstance(&create_info, m_allocator, &m_instance);
        VK_CHECK(res);

#ifdef SPA_DEBUG
        SPA_LOG_DEBUG("Creating Vulkan debug messenger...");

        VkDebugUtilsMessageSeverityFlagsEXT severity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
        //  | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
        //  | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;

        VkDebugUtilsMessageTypeFlagsEXT type =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

        VkDebugUtilsMessengerCreateInfoEXT debug_create_info{};
        debug_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debug_create_info.messageSeverity = severity;
        debug_create_info.messageType = type;
        debug_create_info.pfnUserCallback = vk_debug_callback;
        debug_create_info.pUserData = nullptr; // Optional

        // Load extension function
        auto vkCreateDebugUtilsMessengerEXT =
            reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
                m_instance, "vkCreateDebugUtilsMessengerEXT"));

        SPA_ASSERT_MSG(vkCreateDebugUtilsMessengerEXT, "vkCreateDebugUtilsMessengerEXT not found!");
        res = vkCreateDebugUtilsMessengerEXT(m_instance, &debug_create_info, m_allocator, &m_debug_messenger);
        VK_CHECK(res);

        SPA_LOG_DEBUG("Vulkan debug messenger created.");

#endif

        SPA_LOG_INFO("Vulkan renderer initialized successfully.");

        return true;
    }

    void VulkanBackend::shutdown() {
        SPA_LOG_DEBUG("Destroying Vulkan debugger...");
            if (m_debug_messenger) {
                auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
                        m_instance, "vkDestroyDebugUtilsMessengerEXT"));
                func(m_instance, m_debug_messenger, m_allocator);
            }
        SPA_LOG_DEBUG("Destroying Vulkan instance...");

        vkDestroyInstance(m_instance, m_allocator);
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


VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_types,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data) {
    switch (message_severity) {
        default:
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            SPA_LOG_ERROR(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            SPA_LOG_WARN(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            SPA_LOG_INFO(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            SPA_LOG_TRACE(callback_data->pMessage);
            break;
    }
    return VK_FALSE;
}