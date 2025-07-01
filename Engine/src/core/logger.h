//
// Created by overlord on 6/30/25.
//

#pragma once

#include "defines.h"
#include "spdlog/spdlog.h"

#define SPA_LOG_ENABLE_WARN 1
#define SPA_LOG_ENABLE_INFO 1
#define SPA_LOG_ENABLE_DEBUG 1
#define SPA_LOG_ENABLE_TRACE 1

#ifdef SPA_RELEASE
    #define SPA_LOG_ENABLE_DEBUG 0
    #define SPA_LOG_ENABLE_TRACE 0
#endif


namespace Sparkle {
        class Logger {
        public:
            static bool init();
            static void shutdown();
            static std::shared_ptr<spdlog::logger>& get_logger();

        };

}

#define SPA_LOG_FATAL(...) ::Sparkle::Logger::get_logger()->critical(__VA_ARGS__)
#define SPA_LOG_ERROR(...) ::Sparkle::Logger::get_logger()->error(__VA_ARGS__)

#if SPA_LOG_ENABLE_WARN == 1
#define SPA_LOG_WARN(...)  ::Sparkle::Logger::get_logger()->warn(__VA_ARGS__)
#else
#define SPA_LOG_WARN(...)
#endif

#if SPA_LOG_ENABLE_INFO == 1
#define SPA_LOG_INFO(...)  ::Sparkle::Logger::get_logger()->info(__VA_ARGS__)
#else
#define SPA_LOG_INFO(...)
#endif

#if SPA_LOG_ENABLE_DEBUG == 1
#define SPA_LOG_DEBUG(...) ::Sparkle::Logger::get_logger()->debug(__VA_ARGS__)
#else
#define SPA_LOG_DEBUG(...)
#endif

#if SPA_LOG_ENABLE_TRACE == 1
#define SPA_LOG_TRACE(...) ::Sparkle::Logger::get_logger()->trace(__VA_ARGS__)
#else
#define SPA_LOG_TRACE(...)
#endif