#pragma once

#include <nx/type.h>

/**
 * @brief digest namespace
 */
namespace nx::digest {

/**
 * @brief      MD5 algorithm
 *             ### Example
 *
 *                 uint8_t digest[16];
 *                 MD5 md5;
 *
 *                 md5.update("hello", 5);
 *                 md5.finish(digest);
 *
 */
class MD5 {
public:
    MD5();

    /**
     * @brief      Resets the object.
     */
    void reset();

    /**
     * @brief      append more data to calculate md5
     *
     * @param[in]  input   The input
     * @param[in]  length  The length
     */
    void update(const uint8_t* input, uint32_t length);

    /**
     * @brief      get md5
     *
     * @param      digest  The buffer to accept md5
     */
    void finish(uint8_t digest[16]);

private:
    struct MD5_Context {
        uint32_t total[2];
        uint32_t state[4];
        uint8_t buffer[64];
    };

    MD5_Context context_;

    static void md5_update(MD5_Context* ctx,
                           const uint8_t* input,
                           uint32_t length);
    static void md5_finish(MD5_Context* ctx, uint8_t digest[16]);
    static void md5_process(MD5_Context* ctx, const uint8_t data[64]);
    static void md5_starts(MD5_Context* ctx);
};

/**
 * @brief      This class describes crc 32 algorithm.
 *             ### Example
 *
 *                 uint8_t digest[16];
 *                 CRC32 crc32;
 *
 *                 crc32.update("hello", 5);
 *                 uint32_t checksum = crc32.get_value();
 *
 */
class CRC32 {
public:
    CRC32();

    /**
     * @brief      put data
     *
     * @param[in]  buf   The buffer
     * @param[in]  len   The length
     */
    void update(const uint8_t* buf, size_t len);

    /**
     * @brief      get crc32 value
     *
     * @return     The value.
     */
    uint32_t get_value() const;

private:
    uint32_t table_[256];
    uint32_t initial_;
};

class SHA256 {
public:
    SHA256();

    /**
     * @brief      Resets the object.
     */
    void reset();

    /**
     * @brief      append more data to calculate md5
     *
     * @param[in]  input   The input
     * @param[in]  length  The length
     */
    void update(const uint8_t* input, uint32_t length);

    /**
     * @brief      get md5
     *
     * @param      digest  The buffer to accept md5
     */
    void finish(uint8_t digest[32]);

    struct SHA256_Context {
        uint8_t buf[64];
        uint32_t hash[8];
        uint32_t bits[2];
        uint32_t len;
        uint32_t rfu__;
        uint32_t W[64];
    };

private:
    SHA256_Context context_;
    static void sha256_init(SHA256_Context* ctx);
    static void sha256_hash(SHA256_Context* ctx, const void* data, size_t len);
    static void sha256_done(SHA256_Context* ctx, uint8_t* hash);
    // void sha256(const void *data, size_t len, uint8_t *hash);
};

NX_API String md5(const uint8_t* data, size_t len);
NX_API String md5(const char* data);

NX_API String sha256(const uint8_t* data, size_t len);
NX_API String sha256(const char* data);

NX_API uint32_t crc32(const uint8_t* data, size_t len);
NX_API uint32_t crc32(const char* data);

} // namespace nx::digest
