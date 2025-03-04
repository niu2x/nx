#include <nx/type.h>
#include "log.h"

namespace nx {

void std_error_log(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}

void black_hole(const char* fmt, ...) { (void)fmt; }

PrintLike error_log = std_error_log;

void set_error_log(PrintLike fn) { error_log = fn; }
void set_no_error_log() { error_log = black_hole; }

} // namespace nx