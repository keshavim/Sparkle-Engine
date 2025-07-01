//
// Created by overlord on 6/30/25.
//
#include "spa_pch.h"
#include "logger.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>


namespace Sparkle {
    static std::shared_ptr<spdlog::logger> s_logger;

    bool Logger::init() {
        s_logger = spdlog::stdout_color_mt("sparkle");
        s_logger->set_pattern("[%l] %v");
        s_logger->set_level(spdlog::level::trace);
        return true;
    }

    void Logger::shutdown() {
        spdlog::shutdown();
    }
    std::shared_ptr<spdlog::logger> &Logger::get_logger() {
        return s_logger;
    }

    void report_assertion_failure(const char* expression, const char* message, const char* file, int32_t line) {
        if (auto logger = Logger::get_logger()) {
            logger->critical("Assertion Failure: {}: '{}' [file: {}, line: {}]", expression, message, file, line);
        }
    }
}


