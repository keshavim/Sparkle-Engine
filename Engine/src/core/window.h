//
// Created by overlord on 7/1/25.
//
#pragma once
#include "defines.h"
#include "SDL3/SDL.h"

namespace Sparkle {
    //Rename sdl window flags into equivelent class enums
    enum class WindowFlags : u32 {
        None        = 0,
        Fullscreen  = SDL_WINDOW_FULLSCREEN,
        Hidden      = SDL_WINDOW_HIDDEN,
        Borderless  = SDL_WINDOW_BORDERLESS,
        Resizable   = SDL_WINDOW_RESIZABLE,
        Minimized   = SDL_WINDOW_MINIMIZED,
        Maximized   = SDL_WINDOW_MAXIMIZED,
        HighDPI     = SDL_WINDOW_HIGH_PIXEL_DENSITY,
        Vulkan      = SDL_WINDOW_VULKAN,
        MouseGrab   = SDL_WINDOW_MOUSE_GRABBED
    };

    // Bitwise OR
    inline WindowFlags operator|(WindowFlags a, WindowFlags b) {
        return static_cast<WindowFlags>(static_cast<u32>(a) | static_cast<u32>(b));
    }

    // Bitwise AND
    inline WindowFlags operator&(WindowFlags a, WindowFlags b) {
        return static_cast<WindowFlags>(static_cast<u32>(a) & static_cast<u32>(b));
    }

    // Bitwise OR-assignment
    inline WindowFlags& operator|=(WindowFlags& a, WindowFlags b) {
        a = a | b;
        return a;
    }

    // Convert to SDL flag integer
    inline u32 flagToInt(WindowFlags flags) {
        return static_cast<u32>(flags);
    }

    struct WindowConfig {
        const char* title = "Sparkle App";
        i32 width = 1280;
        i32 height = 720;
        WindowFlags flags = WindowFlags::None;
    };
}