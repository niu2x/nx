#include <nx/type.h>
#include <nx/log.h>
#include <stdarg.h>

#define LOG_LEVEL_GE(a, b) ((int(a)) >= (int(b)))

namespace nx::logging {

static LogLevel get_log_level_from_environment()
{
    auto x = std::getenv("NX_LOG_LEVEL");
    if (x) {
        if (strcasecmp(x, "debug") == 0) {
            return LogLevel::DEBUG;
        } else if (strcasecmp(x, "warning") == 0) {
            return LogLevel::WARNING;
        } else if (strcasecmp(x, "error") == 0) {
            return LogLevel::ERROR;
        } else if (strcasecmp(x, "critical") == 0) {
            return LogLevel::CRITICAL;
        }
    }
    return LogLevel::INFO;
}

static LogLevel current_log_level = get_log_level_from_environment();

void set_level(LogLevel l) { current_log_level = l; }

static const char* tags[] = {
    "DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL",
};

void log_message(LogLevel lv, const char* fmt, ...)
{
    (void)lv;
    (void)fmt;
    (void)tags;
    if (LOG_LEVEL_GE(lv, current_log_level)) {
        va_list args;
        va_start(args, fmt);
        char buffer[1024];
        vsnprintf(buffer, sizeof(buffer), fmt, args);
        fprintf(stderr, "[%s] %s\n", tags[(int)lv], buffer);
        va_end(args);
    }
}

} // namespace nx::logging