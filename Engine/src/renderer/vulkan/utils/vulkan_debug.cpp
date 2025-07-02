//
// Created by overlord on 7/1/25.
//
#include "spa_pch.h"
#include "../vulkan_utils.h"


inline const char *vk_result_to_string(VkResult result) {
    switch (result) {
        case VK_SUCCESS: return "VK_SUCCESS";
        case VK_NOT_READY: return "VK_NOT_READY";
        case VK_TIMEOUT: return "VK_TIMEOUT";
        case VK_EVENT_SET: return "VK_EVENT_SET";
        case VK_EVENT_RESET: return "VK_EVENT_RESET";
        case VK_INCOMPLETE: return "VK_INCOMPLETE";
        case VK_ERROR_OUT_OF_HOST_MEMORY: return "VK_ERROR_OUT_OF_HOST_MEMORY";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
        case VK_ERROR_INITIALIZATION_FAILED: return "VK_ERROR_INITIALIZATION_FAILED";
        case VK_ERROR_DEVICE_LOST: return "VK_ERROR_DEVICE_LOST";
        case VK_ERROR_MEMORY_MAP_FAILED: return "VK_ERROR_MEMORY_MAP_FAILED";
        case VK_ERROR_LAYER_NOT_PRESENT: return "VK_ERROR_LAYER_NOT_PRESENT";
        case VK_ERROR_EXTENSION_NOT_PRESENT: return "VK_ERROR_EXTENSION_NOT_PRESENT";
        case VK_ERROR_FEATURE_NOT_PRESENT: return "VK_ERROR_FEATURE_NOT_PRESENT";
        case VK_ERROR_INCOMPATIBLE_DRIVER: return "VK_ERROR_INCOMPATIBLE_DRIVER";
        case VK_ERROR_TOO_MANY_OBJECTS: return "VK_ERROR_TOO_MANY_OBJECTS";
        case VK_ERROR_FORMAT_NOT_SUPPORTED: return "VK_ERROR_FORMAT_NOT_SUPPORTED";
        case VK_ERROR_FRAGMENTED_POOL: return "VK_ERROR_FRAGMENTED_POOL";
        case VK_ERROR_UNKNOWN: return "VK_ERROR_UNKNOWN";
        case VK_ERROR_OUT_OF_POOL_MEMORY: return "VK_ERROR_OUT_OF_POOL_MEMORY";
        case VK_ERROR_INVALID_EXTERNAL_HANDLE: return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
        case VK_ERROR_FRAGMENTATION: return "VK_ERROR_FRAGMENTATION";
        case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS: return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
        case VK_PIPELINE_COMPILE_REQUIRED: return "VK_PIPELINE_COMPILE_REQUIRED";
        case VK_ERROR_SURFACE_LOST_KHR: return "VK_ERROR_SURFACE_LOST_KHR";
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
        case VK_SUBOPTIMAL_KHR: return "VK_SUBOPTIMAL_KHR";
        case VK_ERROR_OUT_OF_DATE_KHR: return "VK_ERROR_OUT_OF_DATE_KHR";
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
        case VK_ERROR_VALIDATION_FAILED_EXT: return "VK_ERROR_VALIDATION_FAILED_EXT";
        case VK_ERROR_INVALID_SHADER_NV: return "VK_ERROR_INVALID_SHADER_NV";
        case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR: return "VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR";
        case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR: return "VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR";
        case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR: return
                    "VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR";
        case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR: return "VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR";
        case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR: return "VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR";
        case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR: return "VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR";
        case VK_THREAD_IDLE_KHR: return "VK_THREAD_IDLE_KHR";
        case VK_THREAD_DONE_KHR: return "VK_THREAD_DONE_KHR";
        case VK_OPERATION_DEFERRED_KHR: return "VK_OPERATION_DEFERRED_KHR";
        case VK_OPERATION_NOT_DEFERRED_KHR: return "VK_OPERATION_NOT_DEFERRED_KHR";
        case VK_ERROR_COMPRESSION_EXHAUSTED_EXT: return "VK_ERROR_COMPRESSION_EXHAUSTED_EXT";
        default: return "UNKNOWN_VK_RESULT";
    }
}

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
