//
// Created by overlord on 7/2/25.
//

#include "spa_pch.h"
#include "../vulkan_utils.h"

VulkanDevice::~VulkanDevice() {
    Cleanup();
}

// Attempts to initialize the Vulkan device by selecting a suitable physical device.
// The logical device is not created here — that must be done explicitly after this call.
// Returns true if a valid physical device was selected, false otherwise.
bool VulkanDevice::Create(VulkanDevice* out_device, VkInstance& instance, VkSurfaceKHR& surface) {
    out_device->Cleanup();

    if (!out_device->select_physical_device(instance, surface)) {
        SPA_LOG_ERROR("Failed to select suitable physical device.");
        return false;
    }
    if (!out_device->create_logical_device()) {
        SPA_LOG_ERROR("Failed to create logical device.");
        return false;
    }

    return true;
}

void VulkanDevice::Cleanup() {
    if (logical_device_ != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(logical_device_);
        vkDestroyDevice(logical_device_, nullptr);
        logical_device_ = VK_NULL_HANDLE;
    }

    graphicsQueue_ = VK_NULL_HANDLE;
    presentQueue_ = VK_NULL_HANDLE;
    physicalDeviceInfo_ = {};
    swapchainSupport_ = {};
}

// Creates a Vulkan logical device using the selected physical device.
// Sets up the graphics and present queues based on previously discovered indices.
// Also enables required extensions (e.g., swapchain).
// Returns true on success.
bool VulkanDevice::create_logical_device() {
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::unordered_set<u32> uniqueQueueIndices = {
        physicalDeviceInfo_.graphicsQueueIndex,
        physicalDeviceInfo_.presentQueueIndex
    };

    float queuePriority = 1.0f;
    for (u32 index : uniqueQueueIndices) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = index;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<u32>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;

    std::vector<const char*> requiredExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    createInfo.enabledExtensionCount = static_cast<u32>(requiredExtensions.size());
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();

    if (vkCreateDevice(physicalDeviceInfo_.physicalDevice, &createInfo, nullptr, &logical_device_) != VK_SUCCESS) {
        SPA_LOG_ERROR("Failed to create logical device.");
        return false;
    }

    vkGetDeviceQueue(logical_device_, physicalDeviceInfo_.graphicsQueueIndex, 0, &graphicsQueue_);
    vkGetDeviceQueue(logical_device_, physicalDeviceInfo_.presentQueueIndex, 0, &presentQueue_);

    return true;
}

// Queries the given physical device for its surface capabilities, supported formats,
// and present modes on the provided surface. Stores the results for later use.
// Returns true if formats and present modes are available, false otherwise.
bool VulkanDevice::query_swapchain_support(VkPhysicalDevice device, VkSurfaceKHR surface) {
    VulkanSwapchainSupportInfo& info = swapchainSupport_;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &info.capabilities);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
    if (formatCount > 0) {
        info.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, info.formats.data());
    }

    uint32_t presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
    if (presentModeCount > 0) {
        info.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, info.presentModes.data());
    }

    return !info.formats.empty() && !info.presentModes.empty();
}

// Enumerates available physical devices and selects the first suitable one
// based on graphics/present queue availability and swapchain support.
// Populates internal physical device information upon success.
bool VulkanDevice::select_physical_device(VkInstance instance, VkSurfaceKHR surface) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        SPA_LOG_ERROR("No physical devices with Vulkan support found.");
        return false;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for (const auto& device : devices) {
        if (check_device_suitability(device, surface)) {
            physicalDeviceInfo_.physicalDevice = device;
            return true;
        }
    }

    return false;
}

// Determines if the specified physical device meets the requirements
// for rendering, presentation, and swapchain support.
// Stores queue indices and other relevant properties if the device is valid.
bool VulkanDevice::check_device_suitability(VkPhysicalDevice device, VkSurfaceKHR surface) {
    if (device == VK_NULL_HANDLE) return false;

    vkGetPhysicalDeviceProperties(device, &physicalDeviceInfo_.properties);
    vkGetPhysicalDeviceFeatures(device, &physicalDeviceInfo_.features);
    vkGetPhysicalDeviceMemoryProperties(device, &physicalDeviceInfo_.memoryProperties);

    if (!query_swapchain_support(device, surface)) {
        return false;
    }

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    if (queueFamilyCount == 0) return false;

    physicalDeviceInfo_.queueFamilies.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, physicalDeviceInfo_.queueFamilies.data());

    bool graphicsFound = false;
    bool presentFound = false;

    for (uint32_t i = 0; i < queueFamilyCount; ++i) {
        if (physicalDeviceInfo_.queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            physicalDeviceInfo_.graphicsQueueIndex = i;
            graphicsFound = true;
        }

        VkBool32 presentSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if (presentSupport) {
            physicalDeviceInfo_.presentQueueIndex = i;
            presentFound = true;
        }

        if (graphicsFound && presentFound) break;
    }

    return graphicsFound && presentFound;
}
