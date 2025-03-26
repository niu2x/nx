#include <nx/type.h>

namespace nx {

ByteBuffer zlib_compress(const uint8_t* buf, size_t len)
{
    (void)buf;
    (void)len;
#if defined(USE_ZLIB)
    NX_PANIC("build without zlib");
    return {};
#else
    return {};
#endif
}

ByteBuffer zlib_uncompress(const uint8_t* buf, size_t len)
{
    (void)buf;
    (void)len;
#if defined(USE_ZLIB)
    NX_PANIC("build without zlib");
    return {};
#else
    return {};
#endif
}

} // namespace nx
