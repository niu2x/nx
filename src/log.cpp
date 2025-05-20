#include <nx/type.h>
#include <nx/log.h>

namespace nx {

// void std_error_log(const char* fmt, ...)
// {
//     va_list args;
//     va_start(args, fmt);
//     vfprintf(stderr, fmt, args);
//     va_end(args);
// }

// void black_hole(const char* fmt, ...) { (void)fmt; }

// PrintLike error_log = std_error_log;

// void set_error_log(PrintLike fn) { error_log = fn; }
// void set_no_error_log() { error_log = black_hole; }

} // namespace nx

#define LOG_LEVEL_GE(a, b) ((int(a)) >= (int(b)))

namespace nx::logging {

static LogLevel current_log_level = LogLevel::INFO;

void set_level(LogLevel l) { current_log_level = l; }

static const char* tags[] = {
    "DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL",
};

void log_message(LogLevel lv, const char* fmt, ...)
{
    if (LOG_LEVEL_GE(lv, current_log_level)) {
        fprintf(stderr, "[%s] ", tags[(int)lv]);
        va_list args;
        va_start(args, fmt);
        vfprintf(stderr, fmt, args);
        va_end(args);
        fprintf(stderr, "\n");
    }
}

} // namespace nx::logging