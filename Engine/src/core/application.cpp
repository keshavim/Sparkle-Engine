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

        if (!Renderer::initialize()) {
            SPA_LOG_ERROR("Renderer failed to initialize.");
            return false;
        }


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


        RenderPacket packet = {.clearColor = {0.0f, 0.0f, 1.0f, 1.0f}};


        while (m_running) {
            Time::tick();

            Input::begin_frame();

            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                Input::process_event(event);
                if (event.type == SDL_EVENT_QUIT) {
                    m_running = false;
                }
            }


            if (!m_suspended) {
                const f32 dt = Time::delta_time();
                if(!m_game_inst->update(dt)) {
                    SPA_LOG_ERROR("Failed to update");
                    m_running = false;
                }

                packet.deltaTime = dt;
                Renderer::get_backend()->set_clear_color(&packet);


                if (Renderer::draw_frame(&packet)) {
                    if (!m_game_inst->render()) {
                        SPA_LOG_ERROR("Failed to render");
                        m_running = false;
                    }
                }


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
