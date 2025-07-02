//
// Created by overlord on 7/1/25.
//
#include "spa_pch.h"
#include "vulkan_utils.h"
#include "core/logger.h"


inline const char* vk_result_to_string(VkResult result) {
        switch (result) {
            case VK_SUCCESS:                                  return "VK_SUCCESS";
            case VK_NOT_READY:                                return "VK_NOT_READY";
            case VK_TIMEOUT:                                  return "VK_TIMEOUT";
            case VK_EVENT_SET:                                return "VK_EVENT_SET";
            case VK_EVENT_RESET:                              return "VK_EVENT_RESET";
            case VK_INCOMPLETE:                               return "VK_INCOMPLETE";
            case VK_ERROR_OUT_OF_HOST_MEMORY:                 return "VK_ERROR_OUT_OF_HOST_MEMORY";
            case VK_ERROR_OUT_OF_DEVICE_MEMORY:               return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
            case VK_ERROR_INITIALIZATION_FAILED:              return "VK_ERROR_INITIALIZATION_FAILED";
            case VK_ERROR_DEVICE_LOST:                        return "VK_ERROR_DEVICE_LOST";
            case VK_ERROR_MEMORY_MAP_FAILED:                  return "VK_ERROR_MEMORY_MAP_FAILED";
            case VK_ERROR_LAYER_NOT_PRESENT:                  return "VK_ERROR_LAYER_NOT_PRESENT";
            case VK_ERROR_EXTENSION_NOT_PRESENT:              return "VK_ERROR_EXTENSION_NOT_PRESENT";
            case VK_ERROR_FEATURE_NOT_PRESENT:                return "VK_ERROR_FEATURE_NOT_PRESENT";
            case VK_ERROR_INCOMPATIBLE_DRIVER:                return "VK_ERROR_INCOMPATIBLE_DRIVER";
            case VK_ERROR_TOO_MANY_OBJECTS:                   return "VK_ERROR_TOO_MANY_OBJECTS";
            case VK_ERROR_FORMAT_NOT_SUPPORTED:               return "VK_ERROR_FORMAT_NOT_SUPPORTED";
            case VK_ERROR_FRAGMENTED_POOL:                    return "VK_ERROR_FRAGMENTED_POOL";
            case VK_ERROR_UNKNOWN:                            return "VK_ERROR_UNKNOWN";
            case VK_ERROR_OUT_OF_POOL_MEMORY:                 return "VK_ERROR_OUT_OF_POOL_MEMORY";
            case VK_ERROR_INVALID_EXTERNAL_HANDLE:            return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
            case VK_ERROR_FRAGMENTATION:                      return "VK_ERROR_FRAGMENTATION";
            case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:     return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
            case VK_PIPELINE_COMPILE_REQUIRED:                return "VK_PIPELINE_COMPILE_REQUIRED";
            case VK_ERROR_SURFACE_LOST_KHR:                   return "VK_ERROR_SURFACE_LOST_KHR";
            case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:           return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
            case VK_SUBOPTIMAL_KHR:                           return "VK_SUBOPTIMAL_KHR";
            case VK_ERROR_OUT_OF_DATE_KHR:                    return "VK_ERROR_OUT_OF_DATE_KHR";
            case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:           return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
            case VK_ERROR_VALIDATION_FAILED_EXT:              return "VK_ERROR_VALIDATION_FAILED_EXT";
            case VK_ERROR_INVALID_SHADER_NV:                  return "VK_ERROR_INVALID_SHADER_NV";
            case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR:      return "VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR";
            case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR: return "VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR";
            case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR: return "VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR";
            case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR: return "VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR";
            case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR: return "VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR";
            case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR: return "VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR";
            case VK_THREAD_IDLE_KHR:                          return "VK_THREAD_IDLE_KHR";
            case VK_THREAD_DONE_KHR:                          return "VK_THREAD_DONE_KHR";
            case VK_OPERATION_DEFERRED_KHR:                   return "VK_OPERATION_DEFERRED_KHR";
            case VK_OPERATION_NOT_DEFERRED_KHR:               return "VK_OPERATION_NOT_DEFERRED_KHR";
            case VK_ERROR_COMPRESSION_EXHAUSTED_EXT:          return "VK_ERROR_COMPRESSION_EXHAUSTED_EXT";
            default:                                          return "UNKNOWN_VK_RESULT";
        }
    }




bool setup_validation_layers(VkInstanceCreateInfo& create_info) {
#ifdef SPA_DEBUG
    static const std::vector<const char*> validation_layers = {
        "VK_LAYER_KHRONOS_validation"
    };

    uint32_t available_layer_count = 0;
    VkResult res = vkEnumerateInstanceLayerProperties(&available_layer_count, nullptr);
    VK_CHECK(res);

    std::vector<VkLayerProperties> available_layers(available_layer_count);
    res = vkEnumerateInstanceLayerProperties(&available_layer_count, available_layers.data());
    VK_CHECK(res);

    for (const char* required : validation_layers) {
        const bool found = std::ranges::any_of(available_layers, [&](const VkLayerProperties& prop) {
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