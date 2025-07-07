//
// Created by overlord on 7/1/25.
//
#include "spa_pch.h"
#include "vulkan_backend.h"


VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_types,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data);


namespace Sparkle {
    bool VulkanBackend::init() {
        // Setup Vulkan instance.
        VkApplicationInfo app_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
        app_info.apiVersion = VK_API_VERSION_1_3;
        app_info.pApplicationName = Application::GetName();
        app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.pEngineName = "Sparkle Engine";
        app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);

        std::vector<const char*> extension_names =  load_extensions();
        SPA_ASSERT(!extension_names.empty());

        VkInstanceCreateInfo create_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
        create_info.pApplicationInfo = &app_info;
        create_info.enabledExtensionCount = static_cast<u32>(extension_names.size());
        create_info.ppEnabledExtensionNames = extension_names.data();

        setup_validation_layers(create_info);

        VkResult res = vkCreateInstance(&create_info, m_allocator, &m_instance);
        VK_CHECK(res);

        res = setup_debugger(m_instance, m_allocator, m_debug_messenger);
        VK_CHECK(res);
        SPA_LOG_DEBUG("Vulkan debug messenger created.");

        bool result = SDL_Vulkan_CreateSurface(Application::GetWindow(), m_instance, m_allocator, &m_surface);
        SPA_ASSERT(result);
        SPA_LOG_DEBUG("Vulkan surface created");

        res = m_device.create(m_instance, m_surface, m_allocator);
        VK_CHECK(res);
#ifdef SPA_DEBUG
        m_device.test();  // For validation/logging
#endif
        SPA_LOG_DEBUG("Vulkan device created and validated.");

        // Create the swapchain (including views, render pass, depth, and framebuffers)
        res = m_swapchain.create(m_device, m_surface,
                                 Application::GetWidth(),
                                 Application::GetHeight());
        SPA_ASSERT(res == VK_SUCCESS);
#ifdef SPA_DEBUG
         // Test all the internal components for correctness
        m_swapchain.test();
#endif
        SPA_LOG_DEBUG("Swapchain created.");

        // Create sync objects
        res = m_sync_objects.create(m_device.get_logical_device(), m_max_frames_in_flight);
        if (res != VK_SUCCESS) return res;
        SPA_LOG_DEBUG("sync objects created.");


        SPA_LOG_INFO("Vulkan renderer initialized successfully.");

        return true;
    }

    void VulkanBackend::shutdown() {
        SPA_LOG_DEBUG("Waiting for device to be idle...");
        vkDeviceWaitIdle(m_device.get_logical_device());  // ✅ ADD THIS

        SPA_LOG_DEBUG("Destroying sync objects...");
        m_sync_objects.cleanup(m_device.get_logical_device());

        SPA_LOG_DEBUG("Destroying swapchain...");
        m_swapchain.cleanup(m_device.get_logical_device());

        SPA_LOG_DEBUG("Destroying Vulkan devices...");
        m_device.cleanup();

        SPA_LOG_DEBUG("Destroying Vulkan surface...");
        if (m_surface) {
            vkDestroySurfaceKHR(m_instance, m_surface, m_allocator);
            m_surface = VK_NULL_HANDLE;
        }

        SPA_LOG_DEBUG("Destroying Vulkan debugger...");
        if (m_debug_messenger) {
            auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
                vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT"));
            if (func) {
                func(m_instance, m_debug_messenger, m_allocator);
            }
            m_debug_messenger = VK_NULL_HANDLE;
        }

        SPA_LOG_DEBUG("Destroying Vulkan instance...");
        if (m_instance) {
            vkDestroyInstance(m_instance, m_allocator);
            m_instance = VK_NULL_HANDLE;
        }
    }

void VulkanBackend::resize(uint32_t width, uint32_t height) {
    // Wait for the device to be idle before resizing
    vkDeviceWaitIdle(m_device.get_logical_device());

    // Recreate swapchain with new dimensions
    m_swapchain.recreate(m_device, m_surface, width, height);

    // Reset frame index if needed
    m_current_frame = 0;
}

    bool VulkanBackend::begin_frame() {
        VkDevice device = m_device.get_logical_device();

        // Wait on the in-flight fence for the current frame to ensure the previous frame has finished
        VkFence in_flight_fence = m_sync_objects.get_in_flight_fence(m_current_frame);
        vkWaitForFences(device, 1, &in_flight_fence, VK_TRUE, UINT64_MAX);
        vkResetFences(device, 1, &in_flight_fence);

        // Acquire next image from the swapchain
        VkResult result = vkAcquireNextImageKHR(
            device,
            m_swapchain.get_swapchain(),
            UINT64_MAX,
            m_sync_objects.get_image_available_semaphore(m_current_frame),
            VK_NULL_HANDLE,
            &m_current_image_index
        );

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            resize(m_swapchain.get_extent().width, m_swapchain.get_extent().height);  // trigger recreate
            return false;
        }
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            SPA_LOG_ERROR("Failed to acquire swapchain image:");
            printf("\b%d\n", result);
            return false;
        }

        return true;
    }


bool VulkanBackend::end_frame() {
    VkDevice device = m_device.get_logical_device();

    VkSemaphore wait_semaphores[] = { m_sync_objects.get_image_available_semaphore(m_current_frame) };
    VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSemaphore signal_semaphores[] = { m_sync_objects.get_render_finished_semaphore(m_current_frame) };

    VkCommandBuffer command_buffer = m_swapchain.get_command_buffers()[m_current_image_index];

    // Submit command buffer
    VkSubmitInfo submit_info = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    VkFence in_flight_fence = m_sync_objects.get_in_flight_fence(m_current_frame);
    if (vkQueueSubmit(m_device.get_graphics_queue(), 1, &submit_info, in_flight_fence) != VK_SUCCESS) {
        SPA_LOG_ERROR("Failed to submit draw command buffer.");
        return false;
    }

    // Present the rendered image to the screen
    VkPresentInfoKHR present_info = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;

    VkSwapchainKHR swapchains[] = { m_swapchain.get_swapchain() };
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapchains;
    present_info.pImageIndices = &m_current_image_index;
    present_info.pResults = nullptr;

    VkResult result = vkQueuePresentKHR(m_device.get_present_queue(), &present_info);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        resize(m_swapchain.get_extent().width, m_swapchain.get_extent().height);
        return false;
    }
    if (result != VK_SUCCESS) {
        SPA_LOG_ERROR("Failed to present swapchain image: ");
        printf("\b%d\n", result);

        return false;
    }

        m_frame_number++;
    // Advance to next frame in flight
    m_current_frame = (m_current_frame + 1) % m_max_frames_in_flight;
    return true;
}

}

