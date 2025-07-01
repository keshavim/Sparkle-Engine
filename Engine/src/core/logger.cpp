//
// Created by overlord on 6/30/25.
//
#include "spa_pch.h"
#include "logger.h"
#include "assert.h"


namespace Sparkle {
    std::mutex Logger::s_mutex;

    bool Logger::init()
    {
        return true;
    }

    void Logger::shutdown()
    {
        // no-op
    }

    std::string Logger::levelToString(LogLevel level)
    {
        switch (level) {
            case LogLevel::Fatal: return "\x1b[1;31mFATAL\x1b[0m";  // Bold Red
            case LogLevel::Error: return "\x1b[31mERROR\x1b[0m";    // Red
            case LogLevel::Warn:  return "\x1b[33mWARN\x1b[0m";     // Yellow
            case LogLevel::Info:  return "\x1b[32mINFO\x1b[0m";     // Green
            case LogLevel::Debug: return "\x1b[36mDEBUG\x1b[0m";    // Cyan
            case LogLevel::Trace: return "\x1b[90mTRACE\x1b[0m";    // Bright Gray
            default:              return "UNKNOWN";
        }
    }

    void report_assertion_failure(const char* expression, const char* message, const char* file, i32 line) {
        Logger::log_output(LogLevel::Fatal,
            "Assertion Failure: {}: '{}' [file: {}, line: {}]",
            expression, message, file, line);
    }
}


