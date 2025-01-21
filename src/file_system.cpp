#include <nx/file_system.h>

#include <filesystem>

#include "log.h"

namespace std_fs = std::filesystem;

namespace nx::file_system {

bool is_directory(const String& path) { return std_fs::is_directory(path); }

bool is_file(const String& path)
{
    return exists(path) && !(is_directory(path));
}

bool exists(const String& path) { return std_fs::exists(path); }

File::File(const String& p) : path_(p), fp_(nullptr), strong_ref_(true) { }

File::~File() { close(); }

bool File::open(OpenMode m)
{
    if (mode_) {
        error_log("open fail: %s, reason: busy\n", path_.c_str());
        return false;
    }

    fp_ = fopen(path_.c_str(), m == OpenMode::READ ? "rb" : "wb");
    if (fp_ == nullptr) {
        error_log("open fail: %s, reason: fp_ is nullptr\n", path_.c_str());
        return false;
    }

    mode_ = m;
    return true;
}

void File::close()
{
    if (fp_) {
        if (strong_ref_) {
            fclose(fp_);
        }
        fp_ = nullptr;
    }

    mode_ = std::nullopt;
}

File::File(const String& p, FILE* file, OpenMode m)
: path_(p)
, mode_(m)
, fp_(file)
, strong_ref_(false)
{
}

ReadResult File::read(void* buffer, size_t bytes)
{
    if (mode_ != OpenMode::READ) {
        return IO_Error::NOT_OPEN;
    }

    if (feof(fp_))
        return EndOfFile {};

    if (ferror(fp_))
        return IO_Error::IO_FAIL;

    return IO_Success { .bytes = fread(buffer, 1, bytes, fp_) };
}

WriteResult File::write(const void* buffer, size_t bytes)
{
    if (mode_ != OpenMode::WRITE) {
        return IO_Error::NOT_OPEN;
    }

    if (ferror(fp_))
        return IO_Error::IO_FAIL;

    return IO_Success { .bytes = fwrite(buffer, 1, bytes, fp_) };
}

File& File::in()
{
    static File __stdin { "stdin", stdin, OpenMode::READ };
    return __stdin;
}

File& File::out()
{
    static File __stdin { "stdout", stdout, OpenMode::WRITE };
    return __stdin;
}

File& File::err()
{
    static File __stdin { "stderr", stderr, OpenMode::WRITE };
    return __stdin;
}

File& in() { return File::in(); }
File& out() { return File::out(); }
File& err() { return File::err(); }

String get_file_name(const String& path)
{

    return std_fs::path(path).filename();
}

} // namespace nx::file_system