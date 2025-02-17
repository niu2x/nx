#include <nx/file_system.h>

#if defined(USE_STD_FILE_SYSTEM)
    #include <filesystem>
#else
    #include <sys/stat.h>
#endif

#include "log.h"

namespace nx::file_system {

bool is_directory(const String& path)
{
#if defined(USE_STD_FILE_SYSTEM)
    return std::filesystem::is_directory(path);
#else
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        return false; // Cannot access path
    }
    return (info.st_mode & S_IFDIR) != 0;
#endif
}

String get_file_name(const String& path)
{
#if defined(USE_STD_FILE_SYSTEM)
    return std::filesystem::path(path).filename();
#else

    size_t pos = path.find_last_of("/\\");
    if (pos == String::npos) {
        return path; // No directory separators found
    }
    return path.substr(pos + 1);
#endif
}

String get_parent_path(const String& path)
{
#if defined(USE_STD_FILE_SYSTEM)
    return std::filesystem::path(path).parent_path();
#else
    size_t pos = path.find_last_of("/\\");
    if (pos == String::npos) {
        return ""; // No parent path
    }
    return path.substr(0, pos);
#endif
}

bool exists(const String& path)
{
#if defined(USE_STD_FILE_SYSTEM)
    return std::filesystem::exists(path);
#else
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
#endif
}

bool is_file(const String& path)
{
    return exists(path) && !(is_directory(path));
}

// todo: make dirs recursive, if not exists
bool make_dirs(const String& path)
{
    (void)path;
    return false;
}

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

} // namespace nx::file_system