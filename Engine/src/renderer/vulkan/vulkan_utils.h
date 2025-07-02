#pragma once

#include "defines.h"
#include <vulkan/vulkan.h>
#include "core/spa_assert.h"
#include "core/logger.h"
#include "SDL3/SDL_vulkan.h"

#define VK_CHECK(res) do {SPA_ASSERT(res == VK_SUCCESS);} while(false)

inline const char* vk_result_to_string(VkResult result);
std::vector<const char*> load_extensions();
bool setup_validation_layers(VkInstanceCreateInfo& create_info);
VkResult setup_debugger(VkInstance &m_instance, VkAllocationCallbacks* m_allocator, VkDebugUtilsMessengerEXT &m_debug_messenger);



// Holds swapchain capability and supported formats for a physical device.
struct VulkanSwapchainSupportInfo {
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

// Describes a physical device, its properties, and queue family support.
struct VulkanPhysicalDeviceInfo {
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties properties{};
    VkPhysicalDeviceFeatures features{};
    VkPhysicalDeviceMemoryProperties memoryProperties{};
    std::vector<VkQueueFamilyProperties> queueFamilies;

    u32 graphicsQueueIndex = UINT32_MAX;
    u32 presentQueueIndex = UINT32_MAX;

    VulkanSwapchainSupportInfo swapchainSupport;
};

class VulkanDevice {
public:
    VulkanDevice() = default;
    ~VulkanDevice();

    static bool Create(VulkanDevice* out_device, VkInstance& instance, VkSurfaceKHR& surface);
    void Cleanup();

    VkDevice get_handle() const { return logical_device_; }
    VkPhysicalDevice get_physical_device() const { return physicalDeviceInfo_.physicalDevice; }
    VkQueue get_graphics_queue() const { return graphicsQueue_; }
    VkQueue get_present_queue() const { return presentQueue_; }
    u32 get_graphics_queue_index() const { return physicalDeviceInfo_.graphicsQueueIndex; }
    u32 get_present_queue_index() const { return physicalDeviceInfo_.presentQueueIndex; }

    VulkanSwapchainSupportInfo& get_swapchain_support() { return swapchainSupport_;}

    bool create_logical_device();
    bool query_swapchain_support(VkPhysicalDevice device, VkSurfaceKHR surface);

private:
    bool select_physical_device(VkInstance instance, VkSurfaceKHR surface);
    bool check_device_suitability(VkPhysicalDevice device, VkSurfaceKHR surface);

    VulkanPhysicalDeviceInfo physicalDeviceInfo_{};
    VulkanSwapchainSupportInfo swapchainSupport_{};

    VkDevice logical_device_ = VK_NULL_HANDLE;
    VkQueue graphicsQueue_ = VK_NULL_HANDLE;
    VkQueue presentQueue_ = VK_NULL_HANDLE;
};