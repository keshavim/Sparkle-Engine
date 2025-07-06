#pragma once

#include "defines.h"
#include <vulkan/vulkan.h>
#include "core/spa_assert.h"
#include "core/logger.h"
#include "SDL3/SDL_vulkan.h"

#define VK_CHECK(res) do {SPA_ASSERT(res == VK_SUCCESS);} while(false)

std::vector<const char*> load_extensions();
bool setup_validation_layers(VkInstanceCreateInfo& create_info);
VkResult setup_debugger(VkInstance &m_instance, VkAllocationCallbacks* m_allocator, VkDebugUtilsMessengerEXT &m_debug_messenger);

class VulkanDevice {
public:
    VulkanDevice() = default;
    ~VulkanDevice();

    // Create the Vulkan logical device
    VkResult create(VkInstance instance, VkSurfaceKHR surface, VkAllocationCallbacks* allocator);
    void cleanup();
    // Print selected device and queue info for validation
    void test() const;

    // Accessors
    VkDevice get_logical_device() const { return m_device; }
    VkPhysicalDevice get_physical_device() const { return m_physical_device; }
    VkQueue get_graphics_queue() const { return m_graphics_queue; }
    VkQueue get_present_queue() const { return m_present_queue; }
    uint32_t get_graphics_queue_family() const { return m_graphics_queue_family; }
    uint32_t get_present_queue_family() const { return m_present_queue_family; }

private:
    bool is_device_suitable(VkPhysicalDevice device, VkSurfaceKHR surface);
    void pick_physical_device(VkInstance instance, VkSurfaceKHR surface);
    void find_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface);



    VkInstance m_instance = VK_NULL_HANDLE;
    VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
    VkDevice m_device = VK_NULL_HANDLE;

    VkQueue m_graphics_queue = VK_NULL_HANDLE;
    VkQueue m_present_queue = VK_NULL_HANDLE;

    uint32_t m_graphics_queue_family = UINT32_MAX;
    uint32_t m_present_queue_family = UINT32_MAX;

    VkAllocationCallbacks* m_allocator = nullptr;
};



class VulkanSwapchain {
public:
    VulkanSwapchain() = default;
    ~VulkanSwapchain();

    // Create swapchain and image views
    VkResult create(VulkanDevice& device, VkSurfaceKHR surface, uint32_t width, uint32_t height);

    void cleanup(VkDevice device);

    void test() const;

    // Accessors
    VkSwapchainKHR get_swapchain() const { return m_swapchain; }
    const std::vector<VkImageView>& get_image_views() const { return m_image_views; }
    VkFormat get_format() const { return m_image_format; }
    VkExtent2D get_extent() const { return m_extent; }

private:
    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_image_views;
    VkFormat m_image_format{};
    VkExtent2D m_extent{};
};
