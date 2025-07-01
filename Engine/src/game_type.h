//
// Created by overlord on 7/1/25.
//

#pragma once

#include "core/window.h"

//interface for the user create a game instance
namespace Sparkle {
    class Game {
    public:
        WindowConfig config;

        virtual ~Game() = default;

        // Called once to initialize rendering-related state (e.g., shaders, textures)
        virtual bool init() = 0;

        // Called every frame to draw things
        virtual bool render() = 0;

        // Called every frame to update logic
        virtual bool update(float delta_time) = 0;

        // Called when the window is resized
        virtual void on_resize(int new_width, int new_height) = 0;
    private:

    };
}