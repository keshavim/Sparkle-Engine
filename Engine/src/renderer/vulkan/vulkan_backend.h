//
// Created by overlord on 7/1/25.
//

#pragma once

#include "vulkan_utils.h"
#include "renderer/renderer_backend.h"


namespace Sparkle {

    class VulkanBackend : public RenderBackend {
    public:
        VulkanBackend() = default;
        ~VulkanBackend() override = default;

        bool init() override;
        void shutdown() override;
        void resize(uint32_t width, uint32_t height) override;

        bool begin_frame() override;
        bool end_frame() override;

    private:
        VkInstance m_instance = VK_NULL_HANDLE;
        VkAllocationCallbacks* m_allocator = nullptr;
        VkDebugUtilsMessengerEXT m_debug_messenger{};
        VkSurfaceKHR m_surface = VK_NULL_HANDLE;
        VulkanDevice m_device;
        VulkanSwapchain m_swapchain;
    };


} // namespace Sparkle
