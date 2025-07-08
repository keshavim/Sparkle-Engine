//
// Created by overlord on 7/8/25.
//

#pragma once
#include "defines.h"
#include <SDL3/SDL.h>

class Time {
public:
    static void tick();

    static float time();            // Seconds since start
    static float delta_time();      // Delta time (clamped)
    static float unscaled_delta();  // Raw delta time
    static float fps();             // Frames per second
    static uint64_t frame();        // Frame count

private:
    static inline uint64_t s_start_ns = 0;
    static inline uint64_t s_last_ns = 0;
    static inline float s_delta = 0.0f;
    static inline float s_unscaled = 0.0f;
    static inline float s_fps = 0.0f;
    static inline uint64_t s_frame = 0;

    static constexpr float MAX_DELTA = 0.25f; // seconds
};
