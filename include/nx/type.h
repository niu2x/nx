#pragma once

#include <string.h>
#include <stdarg.h>

#include <cstddef>
#include <cstdint>
#include <vector>
#include <map>
#include <type_traits>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <memory>
#include <optional>
#include <variant>
#include <chrono>
#include <string>
#include <stdexcept>
#include <variant>
#include <chrono>
#include <nx/api.h>

#if defined(WIN32)
    #define NX_PLATFORM_WINDOW
#else
    #define NX_PLATFORM_POSIX
#endif

namespace nx {

/**
 * @brief byte array
 */
using ByteBuffer = std::vector<uint8_t>;

template <class T>
using Optional = std::optional<T>;

template <class T>
using Vector = std::vector<T>;

using String = std::string;

template <class... T>
using Variant = std::variant<T...>;

template <class T>
using UniquePtr = std::unique_ptr<T>;

template <class T>
using SharedPtr = std::shared_ptr<T>;

/**
 * @brief      用于被private继承, 子类将不可被Copy
 */
class NX_API Uncopyable {
public:
    Uncopyable() = default;
    ~Uncopyable() = default;

    Uncopyable(const Uncopyable&) = delete;
    Uncopyable& operator=(const Uncopyable&) = delete;

    Uncopyable(Uncopyable&&) = default;
    Uncopyable& operator=(Uncopyable&&) = default;
};


NX_API void panic_fmt(const char* fmt, ...);

constexpr size_t operator""_kb(unsigned long long int n) { return n * 1024; }

/**
 * @brief Open Mode
 */
enum class OpenMode {
    WRITE,
    READ,
};

/**
 * @brief Result of successful io operation
 */
struct IO_Success {
    size_t bytes;
};

/**
 * @brief End of file, a Unit struct.
 */
struct EndOfFile {
};

/**
 * @brief IO Error
 */
enum class IO_Error {
    NOT_OPEN,
    IO_FAIL,
};

/**
 * @brief Result of a read operation
 */
using ReadResult = Variant<IO_Error, EndOfFile, IO_Success>;
using ReadAllResult = Variant<IO_Error, ByteBuffer>;

class NX_API Read {
public:
    virtual ~Read() = 0;
    virtual ReadResult read(void* buffer, size_t bytes) = 0;
    bool read_exact(void* buffer, size_t bytes);
    ReadAllResult read_all();
};

/**
 * @brief Result of a write operation
 */
using WriteResult = Variant<IO_Error, IO_Success>;

class NX_API Write {
public:
    virtual ~Write() = 0;
    virtual WriteResult write(const void* buffer, size_t bytes) = 0;
    virtual bool write_all(const void* buffer, size_t bytes);
};

/**
 * @brief      pipe data from reader to writer
 *
 * @param      reader  The reader
 * @param      writer  The writer
 *
 * @return     success?
 */
NX_API bool pipe(Read& reader, Write& writer);
NX_API bool pipe(Read* reader, Write* writer);

class NX_API MemoryFile : public Read, private Uncopyable {
public:
    MemoryFile(const uint8_t* buffer, size_t buf_len);
    ReadResult read(void* buffer, size_t bytes) override;

private:
    const uint8_t* buffer_;
    size_t buf_len_;
    size_t read_pos_;
};

using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

/**
 * @brief      milliseconds
 */
using TimeDuration = double;

/**
 * @brief      get current clock time
 *
 * @return     The time point.
 */
inline TimePoint time_now() { return std::chrono::system_clock::now(); }

/**
 * @brief      calculate time duration before old and now
 *
 * @param[in]  t_old  The old timepoint
 * @param[in]  t_new  The now timepoint
 *
 * @return     The time duration.
 */
inline TimeDuration time_diff(const TimePoint& t_old, const TimePoint& t_new)
{
    using std_ms = std::chrono::milliseconds;
    return std::chrono::duration_cast<std_ms>(t_new - t_old).count();
}

inline TimeDuration time_diff_epoch(const TimePoint& t)
{
    using std_ms = std::chrono::milliseconds;
    return std::chrono::duration_cast<std_ms>(t.time_since_epoch()).count();
}

inline uint8_t ceil_pow2(uint8_t n)
{
    n -= 1;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    return n + 1;
}

inline uint16_t ceil_pow2(uint16_t n)
{
    n -= 1;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    return n + 1;
}

inline uint32_t ceil_pow2(uint32_t n)
{
    n -= 1;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    return n + 1;
}

inline uint64_t ceil_pow2(uint64_t n)
{
    n -= 1;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n |= n >> 32;
    return n + 1;
}

template<class T>
inline bool is_pow2(T x) { return ((x - 1) & x) == 0; }

using PrintLike = void (*)(const char* fmt, ...);
NX_API void set_error_log(PrintLike fn);
NX_API void set_no_error_log();

NX_API ByteBuffer zlib_compress(const uint8_t* buf, size_t len);
NX_API ByteBuffer zlib_uncompress(const uint8_t* buf, size_t len);

} // namespace nx

/**
 * @brief      panic with a fatal error message.
 *
 * @param      msg   The message
 */
#define NX_PANIC(msg, ...) nx::panic_fmt(msg, ##__VA_ARGS__)

#define NX_ASSERT(cond, msg, ...)                                              \
    if (!(cond)) {                                                             \
        NX_PANIC(msg " file:%s line:%d", ##__VA_ARGS__, __FILE__, __LINE__);   \
    }

/**
 * @brief      get bit flag
 *
 * @param      x     the value
 * @param      bit   The bit index
 *
 * @return     1 or 0
 */
#define NX_GET_BIT(x, bit)      (((x) >> (bit)) & 1)
#define NX_SET_BIT(x, bit)      ((x) | (1 << (bit)))
#define NX_CLEAR_BIT(x, bit)    ((x) & (~(1 << (bit))))

/**
 * @brief      get bit flag as boolean value
 *
 * @param      x     the value
 * @param      bit   The bit index
 *
 * @return     true of false
 */
#define NX_GET_BIT_BOOL(x, bit) (NX_GET_BIT((x), (bit)) == 1)
#define NX_SET_BIT_BOOL(x, bit, value)                                         \
    ((value) ? NX_SET_BIT((x), (bit)) : NX_CLEAR_BIT((x), (bit)))
