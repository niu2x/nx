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

static bool is_terminal_support_color()
{
    const char* term = std::getenv("TERM");
    if (term == nullptr) {
        return false; // 如果没有 TERM 环境变量，终端可能不支持颜色
    }
    std::string term_str(term);
    // 检查 TERM 是否包含支持颜色的标志
    return (term_str.find("xterm") != std::string::npos
            || term_str.find("color") != std::string::npos
            || term_str.find("ansi") != std::string::npos
            || term_str.find("screen") != std::string::npos
            || term_str.find("linux") != std::string::npos);
}

static LogLevel current_log_level = get_log_level_from_environment();
static bool support_color = is_terminal_support_color();

void set_level(LogLevel l) { current_log_level = l; }

static const char* tags[] = {
    "DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL",
};

static const char* tag_colors[] = {
    "", "", "\033[33m", "\033[31m", "\033[91m",
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

        if (support_color) {
            fprintf(stderr, "%s", tag_colors[(int)lv]);
        }
        fprintf(stderr, "[%s] %s\n", tags[(int)lv], buffer);

        if (support_color) {
            fprintf(stderr, "\033[0m");
        }

        va_end(args);
    }
}

} // namespace nx::logging