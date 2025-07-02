//
// Created by overlord on 6/30/25.
//
#pragma once

#include "defines.h"

// Platform-specific debug break
#if defined(_MSC_VER)
    #include <intrin.h>
    #define SPA_DEBUG_BREAK() __debugbreak()
#elif defined(__clang__) || defined(__GNUC__)
    #define SPA_DEBUG_BREAK() __builtin_trap()
#else
    #error "Platform does not support debug break"
#endif

// Enabled flag — comment this out to disable assertions
#ifndef SPA_ASSERTIONS_ENABLED
#define SPA_ASSERTIONS_ENABLED 1
#endif

namespace Sparkle {
    // Reports assertion failure via your logger
    void report_assertion_failure(const char* expression, const char* message, const char* file, i32 line);
}

#if SPA_ASSERTIONS_ENABLED

#define SPA_ASSERT(expr)                                          \
    do {                                                         \
        if (expr) {} else {                                           \
            ::Sparkle::report_assertion_failure(#expr, "", __FILE__, __LINE__); \
            SPA_DEBUG_BREAK();                                   \
        }                                                        \
    } while (false)

#define SPA_ASSERT_MSG(expr, message)                             \
    do {                                                         \
        if (expr) {} else {                                           \
            ::Sparkle::report_assertion_failure(#expr, message, __FILE__, __LINE__); \
            SPA_DEBUG_BREAK();                                   \
        }                                                        \
    } while (false)

#ifdef _DEBUG
#define SPA_ASSERT_DEBUG(expr)                                    \
    do {                                                         \
        if (!(expr)) {                                           \
            ::Sparkle::report_assertion_failure(#expr, "", __FILE__, __LINE__); \
            SPA_DEBUG_BREAK();                                   \
        }                                                        \
    } while (false)
#else
#define SPA_ASSERT_DEBUG(expr) do {} while(false)
#endif

#else

#define SPA_ASSERT(expr)           do {} while(false)
#define SPA_ASSERT_MSG(expr, msg)  do {} while(false)
#define SPA_ASSERT_DEBUG(expr)     do {} while(false)

#endif
