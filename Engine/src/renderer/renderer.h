//
// Created by overlord on 7/1/25.
//

#pragma once

#include "renderer_backend.h"
#include <memory>

namespace Sparkle {

    class Renderer {
    public:
        static bool initialize();
        static void shutdown();

        static bool draw_frame(RenderPacket* packet);

        static RenderBackend* get_backend() { return s_backend.get(); }

    private:
        static bool begin_frame(RenderPacket* packet);
        static bool end_frame(RenderPacket* packet);

        static std::unique_ptr<RenderBackend> s_backend;
    };

} // namespace Sparkle
