//
// Created by overlord on 7/7/25.
//

#pragma once
#include "key_codes.h"

namespace Sparkle {
    class Input {
    public:
        // Called once per frame to reset transient input
        static void begin_frame();

        // Update state from SDL events
        static void process_event(const SDL_Event& event);

        // Query input state
        static bool key_pressed(Key key);
        static bool key_released(Key key);
        static bool key_down(Key key);

        static bool mouse_pressed(MouseButton button);
        static bool mouse_released(MouseButton button);
        static bool mouse_down(MouseButton button);

        // Mouse position
        static int mouse_x();
        static int mouse_y();

        // Scroll
        static int scroll_x();
        static int scroll_y();

    private:
        static constexpr int MAX_KEYS = 512;
        static constexpr int MAX_MOUSE_BUTTONS = 8;

        static bool s_keys_current[MAX_KEYS];
        static bool s_keys_previous[MAX_KEYS];

        static bool s_mouse_current[MAX_MOUSE_BUTTONS];
        static bool s_mouse_previous[MAX_MOUSE_BUTTONS];

        static int s_mouse_x;
        static int s_mouse_y;

        static int s_scroll_x;
        static int s_scroll_y;
    };
}