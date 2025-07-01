//
// Created by overlord on 6/30/25.
//

#pragma once

#include "defines.h"
#include <string>
#include <mutex>
#include <iostream>
#include <format>

#define SPA_LOG_ENABLE_WARN 1
#define SPA_LOG_ENABLE_INFO 1
#define SPA_LOG_ENABLE_DEBUG 1
#define SPA_LOG_ENABLE_TRACE 1

#ifdef SPA_RELEASE
    #define SPA_LOG_ENABLE_DEBUG 0
    #define SPA_LOG_ENABLE_TRACE 0
#endif


namespace Sparkle {
    enum class LogLevel : int {
            Fatal = 0,
            Error = 1,
            Warn  = 2,
            Info  = 3,
            Debug = 4,
            Trace = 5
        };

    class Logger {
    public:


        // Initialize the logger (no-op here, but for interface completeness)
        static bool init();

        // Log output with formatting
        template<typename... Args>
        static void log_output(LogLevel level, const std::string& format_str, Args&&... args);
        // Shutdown the logger (no-op here)
        static void shutdown();

    private:
        static std::mutex s_mutex;

        static std::string levelToString(LogLevel level);
    };


    template<typename... Args>
    void Logger::log_output(LogLevel level, const std::string& format_str, Args&&... args)
    {
        std::lock_guard lock(s_mutex);

        std::string message = std::vformat(format_str, std::make_format_args(std::forward<Args>(args)...));
        std::string output = std::format("[{}] {}\n", levelToString(level), message);

        std::cout << output;
        std::cout.flush();
    }

}

// Logging macros with automatic file and line
#define SPA_LOG_FATAL(fmt, ...) ::Sparkle::Logger::log_output(::Sparkle::LogLevel::Fatal, fmt, ##__VA_ARGS__)
#define SPA_LOG_ERROR(fmt, ...) ::Sparkle::Logger::log_output(::Sparkle::LogLevel::Error, fmt, ##__VA_ARGS__)

#if SPA_LOG_ENABLE_WARN == 1
#define SPA_LOG_WARN(fmt, ...)  ::Sparkle::Logger::log_output(::Sparkle::LogLevel::Warn, fmt, ##__VA_ARGS__)
#else
#define SPA_LOG_WARN(fmt, ...)
#endif

#if SPA_LOG_ENABLE_INFO == 1
#define SPA_LOG_INFO(fmt, ...)  ::Sparkle::Logger::log_output(::Sparkle::LogLevel::Info, fmt, ##__VA_ARGS__)
#else
#define SPA_LOG_INFO(fmt, ...)
#endif

#if SPA_LOG_ENABLE_DEBUG == 1
#define SPA_LOG_DEBUG(fmt, ...) ::Sparkle::Logger::log_output(::Sparkle::LogLevel::Debug, fmt, ##__VA_ARGS__)
#else
#define SPA_LOG_DEBUG(fmt, ...)
#endif

#if SPA_LOG_ENABLE_TRACE == 1
#define SPA_LOG_TRACE(fmt, ...) ::Sparkle::Logger::log_output(::Sparkle::LogLevel::Trace, fmt, ##__VA_ARGS__)
#else
#define SPA_LOG_TRACE(fmt, ...)
#endif
