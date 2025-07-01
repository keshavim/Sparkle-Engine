//
// Created by overlord on 7/1/25.
//

#include "spa_pch.h"
#include "application.h"
#include "logger.h"
#include "spa_assert.h"
#include "renderer/renderer.h"

namespace Sparkle {
    bool Application::_internal_init() {
        Logger::init();
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
            SPA_LOG_ERROR("Failed to initialize SDL: {}", SDL_GetError());
            return false;
        }

        SPA_ASSERT(m_game_inst->init());



        m_window = SDL_CreateWindow(
            m_game_inst->config.title,
            m_game_inst->config.width,
            m_game_inst->config.height,
            flagToInt(m_game_inst->config.flags));

        if (!m_window) {
            SPA_LOG_ERROR("Failed to create SDL window: {}", SDL_GetError());
            SDL_Quit();
            return false;
        }

        Renderer::initialize();


        m_running = true;
        m_suspended = false;


        return true;
    }

    void Application::_internal_shutdown() {
        if (m_window) {
            SDL_DestroyWindow(m_window);
            m_window = nullptr;
        }

        Renderer::shutdown();

        SDL_Quit();
        Logger::shutdown();
    }

    void Application::_internal_run() {
        u64 last_ticks = SDL_GetPerformanceCounter();
        u64 freq = SDL_GetPerformanceFrequency();

        RenderPacket packet;

        while (m_running) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_EVENT_QUIT) {
                    m_running = false;
                }
            }
            const u64 current_ticks = SDL_GetPerformanceCounter();
            const f32 delta_time = static_cast<f32>(current_ticks - last_ticks) / static_cast<f32>(freq);
            last_ticks = current_ticks;


            if (!m_suspended) {
                if(!m_game_inst->update(delta_time)) {
                    SPA_LOG_ERROR("Failed to update");
                    m_running = false;
                }
                if(!m_game_inst->render()) {
                    SPA_LOG_ERROR("Failed to render");
                    m_running = false;
                }

                packet.deltaTime = delta_time;
                Renderer::draw_frame(&packet);


            }
        }

    }

    // Singleton instance
    Application& Application::GetInstance() {
        static Application instance;
        return instance;
    }

    bool Application::Init() {
        return GetInstance()._internal_init();
    }

    void Application::Shutdown() {
        GetInstance()._internal_shutdown();
    }

    void Application::Run() {
        GetInstance()._internal_run();
    }




} // namespace Sparkle
