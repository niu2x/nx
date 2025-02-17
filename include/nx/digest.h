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
    MD5_Context context_;
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

} // namespace nx::digest
