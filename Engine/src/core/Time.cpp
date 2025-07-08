//
// Created by overlord on 7/8/25.
//
#include "spa_pch.h"
#include "Time.h"

void Time::tick() {
    uint64_t now = SDL_GetTicksNS(); // nanoseconds
    if (s_frame == 0) {
        s_start_ns = now;
        s_last_ns = now;
        s_delta = s_unscaled = 0.0f;
        s_fps = 0.0f;
        s_frame = 1;
        return;
    }

    uint64_t diff_ns = now - s_last_ns;
    s_unscaled = diff_ns / 1'000'000'000.0f;
    s_delta = (s_unscaled > MAX_DELTA) ? MAX_DELTA : s_unscaled;
    s_fps = 1.0f / (s_unscaled > 0.0f ? s_unscaled : 0.0001f);
    s_last_ns = now;
    s_frame++;
}

float Time::time() {
    return (SDL_GetTicksNS() - s_start_ns) / 1'000'000'000.0f;
}

float Time::delta_time() { return s_delta; }
float Time::unscaled_delta() { return s_unscaled; }
float Time::fps() { return s_fps; }
uint64_t Time::frame() { return s_frame; }

