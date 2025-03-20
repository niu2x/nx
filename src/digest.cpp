#include <nx/digest.h>

namespace nx::digest {

void hex_encode(const uint8_t* digest, size_t len, char* output)
{
    static char trans[16] = { '0', '1', '2', '3', '4', '5', '6', '7',
                              '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
    for (size_t x = 0; x < len; x++) {
        output[x * 2] = trans[digest[x] >> 4];
        output[x * 2 + 1] = trans[digest[x] & 0x0F];
    }
}

uint32_t crc32(const uint8_t* data, size_t len)
{
    CRC32 crc32_context;
    crc32_context.update(data, len);
    return crc32_context.get_value();
}

uint32_t crc32(const char* data)
{
    return crc32((const uint8_t*)data, strlen(data));
}

String md5(const uint8_t* data, size_t len)
{
    MD5 md5_context;
    md5_context.update(data, len);
    uint8_t digest[16];
    md5_context.finish(digest);

    String result;
    result.resize(32);
    hex_encode(digest, 16, result.data());
    return result;
}

String md5(const char* data) { return md5((const uint8_t*)data, strlen(data)); }

String sha256(const uint8_t* data, size_t len)
{
    SHA256 sha256_context;
    sha256_context.update(data, len);
    uint8_t digest[32];
    sha256_context.finish(digest);

    String result;
    result.resize(64);
    hex_encode(digest, 32, result.data());
    return result;
}

String sha256(const char* data)
{
    return sha256((const uint8_t*)data, strlen(data));
}

} // namespace nx::digest