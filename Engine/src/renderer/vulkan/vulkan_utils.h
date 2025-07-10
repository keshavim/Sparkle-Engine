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

    void test() const;

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


// Manages Vulkan framebuffers for each swapchain image view
class VulkanFramebufferManager {
public:
    VulkanFramebufferManager() = default;
    ~VulkanFramebufferManager();

    // Create one framebuffer per swapchain image
    VkResult create(VkDevice device,
                    const std::vector<VkImageView>& color_views,
                    VkImageView depth_view,
                    VkRenderPass render_pass,
                    VkExtent2D extent);

    // Destroy all framebuffers
    void cleanup(VkDevice device);

    // Accessor
    const std::vector<VkFramebuffer>& get_all() const { return m_framebuffers; }

    // Print test info
    void test() const;

private:
    std::vector<VkFramebuffer> m_framebuffers;
};


// Manages a command pool and command buffers for rendering
class VulkanCommandPool {
public:
    VulkanCommandPool() = default;
    ~VulkanCommandPool();

    // Create the command pool for a specific queue family
    VkResult create(VkDevice device, uint32_t queue_family_index);

    // Allocate primary command buffers
    VkResult allocate_buffers(VkDevice device, uint32_t count);

    // Free and destroy all Vulkan resources
    void cleanup(VkDevice device);

    std::vector<VkCommandBuffer>& get_buffers_mut() { return m_command_buffers; }
    const std::vector<VkCommandBuffer>& get_buffers() const { return m_command_buffers; }

    // Test output
    void test() const;

private:
    VkCommandPool m_pool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> m_command_buffers;
};


class VulkanSwapchain {
public:
    VulkanSwapchain() = default;
    ~VulkanSwapchain();

    // Create swapchain + all related resources
    VkResult create(VulkanDevice& device, VkSurfaceKHR surface, uint32_t width, uint32_t height);

    // Recreate swapchain on resize or other changes
    void recreate(VulkanDevice& device, VkSurfaceKHR surface, uint32_t width, uint32_t height);

    void record_all();
    void record_single(uint32_t image_index);

    // Cleanup all Vulkan resources related to swapchain
    void cleanup(VkDevice device);

    // Run test printout of swapchain and related resources info
    void test() const;

    void set_clear_color(float r, float g, float b, float a);

    VkSwapchainKHR get_swapchain() const { return m_swapchain; }
    VkExtent2D get_extent() const { return m_extent; }
    VkRenderPass get_render_pass() const { return m_render_pass.get(); }
    const std::vector<VkFramebuffer>& get_framebuffers() const { return m_framebuffers.get_all(); }
    const std::vector<VkCommandBuffer>& get_command_buffers() const { return m_command_pool.get_buffers(); }


private:
    // Helper methods to pick best formats and present mode
    VkSurfaceFormatKHR choose_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats);
    VkPresentModeKHR choose_present_mode(const std::vector<VkPresentModeKHR>& available_modes);
    VkExtent2D choose_extent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height);

private:
    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    VkFormat m_format = VK_FORMAT_UNDEFINED;
    VkExtent2D m_extent = {};

    std::vector<VkImage> m_images;
    VkClearValue m_clear_color{};
    VkClearValue m_clear_depth{1.0f, 0.0f};


    VulkanImageViews m_image_views;
    VulkanRenderPass m_render_pass;
    VulkanFramebufferManager m_framebuffers;
    VulkanCommandPool m_command_pool;
};

class VulkanSyncObjects {
public:
    VulkanSyncObjects() = default;
    ~VulkanSyncObjects();

    // Create semaphores and fences for the given number of frames in flight
    VkResult create(VkDevice device, uint32_t max_frames_in_flight);

    // Clean up all synchronization objects
    void cleanup(VkDevice device);

    // Accessors
    VkSemaphore get_image_available_semaphore(uint32_t index) const { return m_image_available_semaphores[index]; }
    VkSemaphore get_render_finished_semaphore(uint32_t index) const { return m_render_finished_semaphores[index]; }
    VkFence get_in_flight_fence(uint32_t index) const { return m_in_flight_fences[index]; }

    uint32_t get_max_frames_in_flight() const { return static_cast<uint32_t>(m_in_flight_fences.size()); }

    // Test output to validate creation
    void test() const;

private:
    std::vector<VkSemaphore> m_image_available_semaphores;
    std::vector<VkSemaphore> m_render_finished_semaphores;
    std::vector<VkFence> m_in_flight_fences;
};
