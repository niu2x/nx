#include <nx/file_system.h>

#include <filesystem>

namespace std_fs = std::filesystem;

namespace nx::file_system {

bool is_directory(const String& path) { return std_fs::is_directory(path); }

bool is_file(const String& path)
{
    return exists(path) && !(is_directory(path));
}

bool exists(const String& path) { return std_fs::exists(path); }

File::File(const String& p) : fp_(nullptr), path_(p) { }

File::~File() { close(); }

bool File::open(OpenMode m)
{
    if (mode_) {
        return false;
    }

    fp_ = fopen(path_.c_str(), "rb");
    if (fp_ == nullptr) {
        return false;
    }

    mode_ = m;
    return true;
}

void File::close()
{
    if (fp_) {
        fclose(fp_);
        fp_ = nullptr;
    }

    mode_ = std::nullopt;
}

} // namespace nx::file_system