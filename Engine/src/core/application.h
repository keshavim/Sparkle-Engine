//
// Created by overlord on 7/1/25.
//

#pragma once

#include "window.h"
#include "../game_type.h"
#include <SDL3/SDL.h>
#include "input.h"
#include "Time.h"
namespace Sparkle {
    class Application {
    public:
        // Public static API
        static bool Init();
        static void Shutdown();
        static void Run();

        static bool IsRunning() { return GetInstance().m_running; }
        static bool IsSuspended() { return GetInstance().m_suspended; }

        static SDL_Window* GetWindow() { return GetInstance().m_window; }
        static const char* GetName(){return GetInstance().m_game_inst->config.title;}
        static i32 GetWidth() {return GetInstance().m_game_inst->config.width;}
        static i32 GetHeight() {return GetInstance().m_game_inst->config.height;}

        static void SetGameInst(Game *game) { GetInstance().m_game_inst = game; }

    private:
        // Singleton access
        static Application& GetInstance();

        // Internal instance methods
        bool _internal_init();
        void _internal_shutdown();
        void _internal_run();

        // Prevent construction
        Application() = default;
        ~Application() = default;
        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;
        Application(Application&&) = delete;
        Application& operator=(Application&&) = delete;

    private:
        Game *m_game_inst = nullptr;
        SDL_Window* m_window = nullptr;
        bool m_running = false;
        bool m_suspended = false;
    };

} // namespace Sparkle
