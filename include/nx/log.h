#pragma once

#include <nx/type.h>

#if NX_PLATFORM == NX_PLATFORM_WINDOW
    #undef ERROR
#endif

namespace nx::logging {

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL,
};

NX_API void set_level(LogLevel l);
NX_API void log_message(LogLevel l, const char* msg, ...);

} // namespace nx::logging

#define NX_LOG_DEBUG(...)    NX_LOG_IMPL(DEBUG, __VA_ARGS__)
#define NX_LOG_INFO(...)     NX_LOG_IMPL(INFO, __VA_ARGS__)
#define NX_LOG_WARNING(...)  NX_LOG_IMPL(WARNING, __VA_ARGS__)
#define NX_LOG_ERROR(...)    NX_LOG_IMPL(ERROR, __VA_ARGS__)
#define NX_LOG_CRITICAL(...) NX_LOG_IMPL(CRITICAL, __VA_ARGS__)

#define NX_LOG_IMPL(level, ...)                                                \
    nx::logging::log_message(nx::logging::LogLevel::level, __VA_ARGS__)
