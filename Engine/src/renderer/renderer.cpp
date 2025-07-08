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

    bool Renderer::begin_frame(RenderPacket* packet) {
        return s_backend->begin_frame(packet);
    }

    bool Renderer::end_frame(RenderPacket* packet) {
        bool result = s_backend->end_frame(packet);
        return result;
    }

    bool Renderer::draw_frame(RenderPacket* packet) {
        if (begin_frame(packet)) {

            bool result = end_frame(packet);
            if (!result) {
                SPA_LOG_ERROR("renderer_end_frame failed. Application shutting down...");
                return false;
            }
        }
        return true;
    }

} // namespace Sparkle
