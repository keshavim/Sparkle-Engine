//
// Created by overlord on 7/2/25.
//

#include "spa_pch.h"
#include "../vulkan_utils.h"

VulkanDevice::~VulkanDevice() {
    if (m_device) {
        vkDestroyDevice(m_device, m_allocator);
    }
}

VkResult VulkanDevice::create(VkInstance instance, VkSurfaceKHR surface, VkAllocationCallbacks* allocator) {
    m_instance = instance;
    m_allocator = allocator;

    // Select a GPU that supports required features and presentation
    pick_physical_device(instance, surface);

    // Specify queues to create
    float queue_priority = 1.0f;
    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::vector<uint32_t> unique_families = {m_graphics_queue_family};

    if (m_graphics_queue_family != m_present_queue_family)
        unique_families.push_back(m_present_queue_family);

    for (uint32_t family : unique_families) {
        VkDeviceQueueCreateInfo queue_info = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
        queue_info.queueFamilyIndex = family;
        queue_info.queueCount = 1;
        queue_info.pQueuePriorities = &queue_priority;
        queue_create_infos.push_back(queue_info);
    }

    // Enable swapchain extension
    const char* device_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    VkDeviceCreateInfo create_info = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
    create_info.pQueueCreateInfos = queue_create_infos.data();
    create_info.enabledExtensionCount = 1;
    create_info.ppEnabledExtensionNames = device_extensions;

    VkResult result = vkCreateDevice(m_physical_device, &create_info, m_allocator, &m_device);
    if (result != VK_SUCCESS) return result;

    // Retrieve queues
    vkGetDeviceQueue(m_device, m_graphics_queue_family, 0, &m_graphics_queue);
    vkGetDeviceQueue(m_device, m_present_queue_family, 0, &m_present_queue);

    return VK_SUCCESS;
}

void VulkanDevice::pick_physical_device(VkInstance instance, VkSurfaceKHR surface) {
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
    assert(device_count > 0 && "No Vulkan-compatible devices found!");

    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

    for (const auto& device : devices) {
        if (is_device_suitable(device, surface)) {
            m_physical_device = device;
            return;
        }
    }

    assert(false && "Failed to find a suitable GPU!");
}

bool VulkanDevice::is_device_suitable(VkPhysicalDevice device, VkSurfaceKHR surface) {
    find_queue_families(device, surface);
    return m_graphics_queue_family != UINT32_MAX && m_present_queue_family != UINT32_MAX;
}

void VulkanDevice::find_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface) {
    uint32_t queue_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_count, nullptr);
    std::vector<VkQueueFamilyProperties> properties(queue_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_count, properties.data());

    for (uint32_t i = 0; i < queue_count; ++i) {
        if (properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            m_graphics_queue_family = i;

        VkBool32 present_support = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present_support);
        if (present_support)
            m_present_queue_family = i;

        if (m_graphics_queue_family != UINT32_MAX && m_present_queue_family != UINT32_MAX)
            return;
    }
}

void VulkanDevice::cleanup() {
    if (m_device != VK_NULL_HANDLE) {
        vkDestroyDevice(m_device, m_allocator);
        m_device = VK_NULL_HANDLE;
    }
    m_physical_device = VK_NULL_HANDLE;
    m_graphics_queue = VK_NULL_HANDLE;
    m_present_queue = VK_NULL_HANDLE;
    m_graphics_queue_family = UINT32_MAX;
    m_present_queue_family = UINT32_MAX;
}


void VulkanDevice::test() const {
    assert(m_physical_device && "Device not initialized!");

    VkPhysicalDeviceProperties props{};
    vkGetPhysicalDeviceProperties(m_physical_device, &props);

    std::cout << "Selected GPU: " << props.deviceName << "\n";
    std::cout << "API Version: "
              << VK_VERSION_MAJOR(props.apiVersion) << "."
              << VK_VERSION_MINOR(props.apiVersion) << "."
              << VK_VERSION_PATCH(props.apiVersion) << "\n";

    std::cout << "Graphics Queue Family Index: " << m_graphics_queue_family << "\n";
    std::cout << "Present Queue Family Index: " << m_present_queue_family << "\n";
}
