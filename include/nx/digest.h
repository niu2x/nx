#pragma once

#include <nx/type.h>

/**
 * @brief digest namespace
 */
namespace nx::digest {

struct MD5_Context {
    uint32_t total[2];
    uint32_t state[4];
    uint8_t buffer[64];
};

void md5_starts(struct MD5_Context* ctx);
void md5_update(struct MD5_Context* ctx, uint8_t* input, uint32_t length);
void md5_finish(struct MD5_Context* ctx, uint8_t digest[16]);

/* Uses a static buffer, so beware of how it's used. */
const char* md5_ascii_str(uint8_t digest[16]);

} // namespace nx::digest
