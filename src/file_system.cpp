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

} // namespace nx::file_system