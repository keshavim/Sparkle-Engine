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


// Encapsulates color + depth image views used by the swapchain
class VulkanImageViews {
public:
    VulkanImageViews() = default;
    ~VulkanImageViews();

    // Creates image views for swapchain images + depth image/view
    VkResult create(VulkanDevice& device, const std::vector<VkImage>& images, VkFormat color_format, VkExtent2D extent);
    void cleanup(VkDevice device);

    const std::vector<VkImageView>& get_color_views() const { return m_color_views; }
    VkImageView get_depth_view() const { return m_depth_view; }
    VkFormat get_depth_format() const { return m_depth_format; }

private:
    // Swapchain image views
    std::vector<VkImageView> m_color_views;

    // Depth buffer
    VkImage m_depth_image = VK_NULL_HANDLE;
    VkDeviceMemory m_depth_memory = VK_NULL_HANDLE;
    VkImageView m_depth_view = VK_NULL_HANDLE;
    VkFormat m_depth_format{};

    VkFormat choose_depth_format(VkPhysicalDevice phys);
};


// Encapsulates a Vulkan render pass that supports one color and one depth attachment
class VulkanRenderPass {
public:
    VulkanRenderPass() = default;
    ~VulkanRenderPass();

    // Create a render pass with given color + depth formats
    VkResult create(VkDevice device, VkFormat color_format, VkFormat depth_format);
    void cleanup(VkDevice device);

    VkRenderPass get() const { return m_render_pass; }

private:
    VkRenderPass m_render_pass = VK_NULL_HANDLE;
};


class VulkanSwapchain {
public:
    VulkanSwapchain() = default;
    ~VulkanSwapchain();

    // Create or recreate the swapchain and all associated views, render pass, and framebuffers
    VkResult create(VulkanDevice& device, VkSurfaceKHR surface, uint32_t width, uint32_t height);
    VkResult recreate(VulkanDevice& device, VkSurfaceKHR surface, uint32_t width, uint32_t height);
    void cleanup(VkDevice device);

    void test() const;

    // Accessors
    VkSwapchainKHR get_swapchain() const { return m_swapchain; }
    VkExtent2D get_extent() const { return m_extent; }
    VkFormat get_image_format() const { return m_image_format; }
    VkRenderPass get_render_pass() const { return m_render_pass.get(); }
    const std::vector<VkFramebuffer>& get_framebuffers() const { return m_framebuffers; }

private:
    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> m_images;
    std::vector<VkFramebuffer> m_framebuffers;

    VkFormat m_image_format{};
    VkExtent2D m_extent{};

    VulkanImageViews m_image_views;
    VulkanRenderPass m_render_pass;

    VkSurfaceFormatKHR choose_surface_format(const std::vector<VkSurfaceFormatKHR>& formats);
    VkPresentModeKHR choose_present_mode(const std::vector<VkPresentModeKHR>& modes);
    VkExtent2D choose_extent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height);
};
