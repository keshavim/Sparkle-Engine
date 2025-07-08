//
// Created by overlord on 7/7/25.
//
#include "spa_pch.h"
#include "input.h"

namespace Sparkle {
    bool Input::s_keys_current[MAX_KEYS] = {};
    bool Input::s_keys_previous[MAX_KEYS] = {};

    bool Input::s_mouse_current[MAX_MOUSE_BUTTONS] = {};
    bool Input::s_mouse_previous[MAX_MOUSE_BUTTONS] = {};

    int Input::s_mouse_x = 0;
    int Input::s_mouse_y = 0;

    int Input::s_scroll_x = 0;
    int Input::s_scroll_y = 0;

    void Input::begin_frame() {
        std::memcpy(s_keys_previous, s_keys_current, sizeof(s_keys_current));
        std::memcpy(s_mouse_previous, s_mouse_current, sizeof(s_mouse_current));
        s_scroll_x = 0;
        s_scroll_y = 0;
    }

    void Input::process_event(const SDL_Event &event) {
        switch (event.type) {
            case SDL_EVENT_KEY_DOWN:
                if (event.key.repeat == 0)
                    s_keys_current[event.key.scancode] = true;
                break;

            case SDL_EVENT_KEY_UP:
                s_keys_current[event.key.scancode] = false;
                break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                s_mouse_current[event.button.button] = true;
                break;

            case SDL_EVENT_MOUSE_BUTTON_UP:
                s_mouse_current[event.button.button] = false;
                break;

            case SDL_EVENT_MOUSE_MOTION:
                s_mouse_x = event.motion.x;
                s_mouse_y = event.motion.y;
                break;

            case SDL_EVENT_MOUSE_WHEEL:
                s_scroll_x += event.wheel.x;
                s_scroll_y += event.wheel.y;
                break;

            default:
                break;
        }
    }

    // === Keyboard ===
    bool Input::key_pressed(Key key) {
        int scancode = static_cast<int>(key);
        return s_keys_current[scancode] && !s_keys_previous[scancode];
    }

    bool Input::key_released(Key key) {
        int scancode = static_cast<int>(key);
        return !s_keys_current[scancode] && s_keys_previous[scancode];
    }

    bool Input::key_down(Key key) {
        int scancode = static_cast<int>(key);
        return s_keys_current[scancode];
    }

    // === Mouse ===
    bool Input::mouse_pressed(MouseButton button) {
        int idx = static_cast<int>(button);
        return s_mouse_current[idx] && !s_mouse_previous[idx];
    }

    bool Input::mouse_released(MouseButton button) {
        int idx = static_cast<int>(button);
        return !s_mouse_current[idx] && s_mouse_previous[idx];
    }

    bool Input::mouse_down(MouseButton button) {
        int idx = static_cast<int>(button);
        return s_mouse_current[idx];
    }

    int Input::mouse_x() { return s_mouse_x; }
    int Input::mouse_y() { return s_mouse_y; }

    int Input::scroll_x() { return s_scroll_x; }
    int Input::scroll_y() { return s_scroll_y; }
}
