#include <nx/type.h>
#include "log.h"

class RuntimeException : public std::exception {
public:
    explicit RuntimeException(const std::string& message)
    : message_(message) { }
    const char* what() const noexcept override { return message_.c_str(); }

private:
    std::string message_;
};

namespace nx {

void panic_fmt(const char* fmt, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    throw RuntimeException(buffer);
}

void std_error_log(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}

PrintLike error_log = std_error_log;

void set_error_log(PrintLike fn) { error_log = fn; }

} // namespace nx