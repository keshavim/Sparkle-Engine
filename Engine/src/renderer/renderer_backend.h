//
// Created by overlord on 7/1/25.
//

#pragma once

#include "defines.h"
#include "core/application.h"



namespace Sparkle {

    class RenderBackend {
    public:
        virtual ~RenderBackend() = default;

        virtual bool init() = 0;
        virtual void shutdown() = 0;
        virtual void resize(uint32_t width, uint32_t height) = 0;

        virtual bool begin_frame() = 0;
        virtual bool end_frame() = 0;

        uint64_t m_frame_number = 0;
    protected:
        Application* m_state = nullptr;

    };

    struct RenderPacket {
        f32 deltaTime = 0.0f;
    };

} // namespace Sparkle
