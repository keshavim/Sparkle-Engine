//
// Created by overlord on 6/30/25.
//
#pragma once


#include <cstdint>
// Short typedefs for fixed-width integers
using i8 = int8_t;
using u8 = uint8_t;
using i16 = int16_t;
using u16 = uint16_t;
using i32 = int32_t;
using u32 = uint32_t;
using i64 = int64_t;
using u64 = uint64_t;

// Floating point typedefs
using f32 = float;
using f64 = double;


// Useful macros
#define BIT(x) (1 << (x))
#define UNUSED(x) (void)(x)

// Properly define static assertions.
#if defined(__clang__) || defined(__gcc__)
    #define STATIC_ASSERT _Static_assert
#else
    #define STATIC_ASSERT static_assert
#endif


// Platform detection
#if defined(_WIN32)
    #define SPA_PLATFORM_WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
    #define SPA_PLATFORM_MACOS
#elif defined(__linux__)
    #define SPA_PLATFORM_LINUX
#else
    #error "SPA: Unsupported platform"
#endif

// 64-bit check
#if defined(__x86_64__) || defined(_M_X64) || defined(__aarch64__)
    // ok
#else
    #error "SPA: 32-bit builds are not supported"
#endif

// Compiler detection
#if defined(_MSC_VER)
    #define SPA_COMPILER_MSVC
#elif defined(__clang__)
    #define SPA_COMPILER_CLANG
#elif defined(__GNUC__)
    #define SPA_COMPILER_GCC
#else
    #error "SPA: Unsupported compiler"
#endif

// Symbol visibility
#if defined(SPA_PLATFORM_WINDOWS)
    #if defined(SPA_EXPORTS)
        #define SPA_API __declspec(dllexport)
    #else
        #define SPA_API __declspec(dllimport)
    #endif
#else
    #if __GNUC__ >= 4
        #define SPA_API __attribute__((visibility("default")))
    #else
        #define SPA_API
    #endif
#endif


/**
 * @brief Indicates if the provided flag is set in the given flags int.
 */
#define SPA_FLAG_GET(flags, flag) ((flags & flag) == flag)

/**
 * @brief Sets a flag within the flags int to enabled/disabled.
 *
 * @param flags The flags int to write to.
 * @param flag The flag to set.
 * @param enabled Indicates if the flag is enabled or not.
 */
#define SPA_FLAG_SET(flags, flag, enabled) (flags = (enabled ? (flags | flag) : (flags & ~flag)))