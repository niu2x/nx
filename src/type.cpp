#include <nx/type.h>

namespace nx {

Read::~Read() { }

bool Read::read_exact(void* buffer, size_t bytes)
{
    auto* ptr = (uint8_t*)buffer;
    while (bytes) {
        auto result = read(ptr, bytes);
        if (std::holds_alternative<IO_Success>(result)) { }
        if (auto* r = std::get_if<IO_Success>(&result)) {
            bytes -= r->bytes;
            ptr += r->bytes;
        } else {
            return false;
        }
    }
    return true;
}

Write::~Write() { }

bool Write::write_all(const void* buffer, size_t bytes)
{
    auto* ptr = (const uint8_t*)buffer;
    WriteResult result;
    size_t offset = 0;
    while (bytes) {
        result = write(ptr + offset, bytes);

        bool io_fail = false;
        size_t io_bytes = 0;

        std::visit(
            [&io_fail, &io_bytes](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, IO_Error>) {
                    io_fail = true;
                } else if constexpr (std::is_same_v<T, IO_Success>) {
                    io_bytes = arg.bytes;
                } else {
                    NX_PANIC("non-exhaustive visitor!");
                }
            },
            result);

        if (io_fail)
            return false;

        if (io_bytes > 0) {
            offset += io_bytes;
            bytes -= io_bytes;
        }
    }

    return true;
}

bool pipe(Read& source, Write& sink)
{
    const auto buf_nr = 8_kb;
    uint8_t buffer[buf_nr];
    ReadResult read_result;
    while (true) {
        read_result = source.read(buffer, buf_nr);

        bool read_fail = false;
        bool eof = false;
        size_t bytes = 0;

        std::visit(
            [&read_fail, &eof, &bytes](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, EndOfFile>) {
                    eof = true;
                } else if constexpr (std::is_same_v<T, IO_Error>) {
                    read_fail = true;
                } else if constexpr (std::is_same_v<T, IO_Success>) {
                    bytes = arg.bytes;
                } else {
                    NX_PANIC("non-exhaustive visitor!");
                }
            },
            read_result);

        if (eof)
            break;

        if (read_fail)
            return false;

        if (bytes > 0) {
            if (!sink.write_all(buffer, bytes))
                return false;
        }
    };

    return true;
}

MemoryFile::MemoryFile(const uint8_t* buffer, size_t buf_len)
: buffer_(buffer)
, buf_len_(buf_len)
, read_pos_(0)
{
}

ReadResult MemoryFile::read(void* output, size_t n)
{
    size_t remain = buf_len_ - read_pos_;

    if (remain == 0) {
        return EndOfFile {};
    }

    n = std::min(n, remain);

    memcpy(output, buffer_ + read_pos_, n);
    read_pos_ += n;
    return IO_Success { n };
}

} // namespace nx