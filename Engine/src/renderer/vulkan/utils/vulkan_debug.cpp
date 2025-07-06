//
// Created by overlord on 7/1/25.
//
#include "spa_pch.h"
#include "../vulkan_utils.h"


std::vector<const char*> load_extensions() {
    //loading extentions
    Uint32 extension_count = 0;

    // Get the extension list from SDL
    const char * const * extensions = SDL_Vulkan_GetInstanceExtensions(&extension_count);
    if (!extensions) {
        SPA_LOG_ERROR("SDL_Vulkan_GetInstanceExtensions failed: {}", SDL_GetError());
        return {};
    }
    std::vector<const char*> extension_names(extensions, extensions + extension_count);
#ifdef SPA_DEBUG
    extension_names.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    extension_names.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    SPA_LOG_DEBUG("Vulkan_Extensions");
    for (auto extension_name : extension_names)
        SPA_LOG_DEBUG("{}", extension_name);
#endif

    return extension_names;
}


bool setup_validation_layers(VkInstanceCreateInfo &create_info) {
#ifdef SPA_DEBUG
    static const std::vector<const char *> validation_layers = {
        "VK_LAYER_KHRONOS_validation"
    };

    uint32_t available_layer_count = 0;
    VkResult res = vkEnumerateInstanceLayerProperties(&available_layer_count, nullptr);
    VK_CHECK(res);

    std::vector<VkLayerProperties> available_layers(available_layer_count);
    res = vkEnumerateInstanceLayerProperties(&available_layer_count, available_layers.data());
    VK_CHECK(res);

    for (const char *required: validation_layers) {
        const bool found = std::ranges::any_of(available_layers, [&](const VkLayerProperties &prop) {
            return std::strcmp(prop.layerName, required) == 0;
        });

        if (!found) {
            SPA_LOG_ERROR("Required validation layer not found: {}", required);
            return false;
        }
    }

    create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
    create_info.ppEnabledLayerNames = validation_layers.data();
#else
    create_info.enabledLayerCount = 0;
    create_info.ppEnabledLayerNames = nullptr;
#endif

    return true;
}


VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_types,
    const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
    void *user_data) {
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

VkResult setup_debugger(VkInstance &m_instance, VkAllocationCallbacks *m_allocator,
                    VkDebugUtilsMessengerEXT &m_debug_messenger) {
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
    VkResult res = vkCreateDebugUtilsMessengerEXT(m_instance, &debug_create_info, m_allocator, &m_debug_messenger);

    return res;
#endif
}
