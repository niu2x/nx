#include <cstdint>
#include <cstddef>
#include <nx/digest.h>

namespace nx::digest {

void __generate_table(uint32_t table[256])
{
    uint32_t polynomial = 0xEDB88320;
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t c = i;
        for (size_t j = 0; j < 8; j++) {
            if (c & 1) {
                c = polynomial ^ (c >> 1);
            } else {
                c >>= 1;
            }
        }
        table[i] = c;
    }
}

uint32_t
__update(uint32_t table[256], uint32_t initial, const void* buf, size_t len)
{
    uint32_t c = initial ^ 0xFFFFFFFF;
    const uint8_t* u = static_cast<const uint8_t*>(buf);
    for (size_t i = 0; i < len; ++i) {
        c = table[(c ^ u[i]) & 0xFF] ^ (c >> 8);
    }
    return c ^ 0xFFFFFFFF;
}

CRC32::CRC32() : initial_(0) { __generate_table(table_); }

void CRC32::update(const uint8_t* buf, size_t len)
{
    initial_ = __update(table_, initial_, (const void*)buf, len);
}

uint32_t CRC32::get_value() const { return initial_; }

} // namespace nx::digest