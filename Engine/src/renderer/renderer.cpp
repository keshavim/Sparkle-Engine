//
// Created by overlord on 7/1/25.
//

#include "renderer.h"
#include "vulkan/vulkan_backend.h"
#include <memory>
#include "core/logger.h"

namespace Sparkle {

    std::unique_ptr<RenderBackend> Renderer::s_backend = nullptr;

    bool Renderer::initialize() {
        s_backend = std::make_unique<VulkanBackend>();
        s_backend->m_frame_number = 0;

        if (!s_backend->init()) {
            SPA_LOG_FATAL("Renderer backend failed to initialize. Shutting down.");
            return false;
        }

        return true;
    }

    void Renderer::shutdown() {
        if (s_backend) {
            s_backend->shutdown();
            s_backend.reset();
        }
    }

    bool Renderer::begin_frame(float delta_time) {
        return s_backend->begin_frame();
    }

    bool Renderer::end_frame(float delta_time) {
        bool result = s_backend->end_frame();
        s_backend->m_frame_number++;
        return result;
    }

    bool Renderer::draw_frame(RenderPacket* packet) {
        if (begin_frame(packet->deltaTime)) {
            bool result = end_frame(packet->deltaTime);
            if (!result) {
                SPA_LOG_ERROR("renderer_end_frame failed. Application shutting down...");
                return false;
            }
        }
        return true;
    }

} // namespace Sparkle
