//
// Created by overlord on 7/1/25.
//

#pragma once

#include "defines.h"
#include <vulkan/vulkan.h>
#include "core/application.h"



namespace Sparkle {
    struct RenderPacket {
        f32 deltaTime = 0.0f;
        float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    };

    class RenderBackend {
    public:
        virtual ~RenderBackend() = default;

        virtual bool init() = 0;
        virtual void shutdown() = 0;
        virtual void resize(uint32_t width, uint32_t height) = 0;

        virtual bool begin_frame(const RenderPacket* packet) = 0;
        virtual bool end_frame(const RenderPacket* packet) = 0;

        virtual void set_clear_color(const RenderPacket* packet) = 0;

        uint64_t get_frame_number() const { return m_frame_number; }
        uint32_t get_current_frame() const { return m_current_frame; }
        uint32_t get_current_image_index() const { return m_current_image_index; }


    protected:
        uint32_t m_max_frames_in_flight = 2;
        uint32_t m_current_frame = 0;
        uint32_t m_current_image_index = 0;
        uint64_t m_frame_number = 0;


    };



} // namespace Sparkle
