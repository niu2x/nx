#include <nx/type.h>
#include <nx/log.h>

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
    NX_LOG_CRITICAL(buffer);
    throw RuntimeException(buffer);
}

} // namespace nx