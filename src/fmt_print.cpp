#include <nx/type.h>

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

} // namespace nx