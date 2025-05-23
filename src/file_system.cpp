#include <nx/file_system.h>

#if defined(USE_STD_FILE_SYSTEM)
    #include <filesystem>
#else
    #include <sys/stat.h>
#endif

#if NX_PLATFORM_WINDOW == NX_PLATFORM
    #include <process.h>
    #include <io.h>
    #include <direct.h>
    #define access _access
    #define F_OK 0
    #define mkdir(a, b) _mkdir((a))
#else
    #include <unistd.h>
    #include <sys/stat.h>
#endif

#include <sstream>
#include <nx/log.h>

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
    return std::filesystem::path(path).filename().u8string();
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
    return std::filesystem::path(path).parent_path().u8string();
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

bool make_dirs(const String& path)
{
    // 空路径，返回true
    if (path.empty()) {
        return true;
    }

    // 检查路径是否存在
    if (access(path.c_str(), F_OK) != -1) {
        // 已存在，返回true
        return true;
    }

    // 找到最后一个'/'的位置
    size_t last_slash = path.rfind('/');

    // 分割父目录和当前目录
    String parent_dir, current_dir;

    if (last_slash == String::npos) {
        // 没有'/'，表示在当前目录下，直接创建
        // 但需要确保父目录存在，即当前路径是否正确
        // 其实，这种情况下，parent_dir应该是当前路径的前缀，比如可能为空
        // 但可能需要递归调用父目录
        // 这里可能需要重新考虑，比如，将parent_dir设为path的父目录，可能为空
        // 这时，current_dir是整个path
        parent_dir = "";
        current_dir = path;
    } else {
        parent_dir = path.substr(0, last_slash);
        current_dir = path.substr(last_slash + 1);

        // 特殊情况处理，如果路径以'/'结尾，current_dir可能为空
        if (current_dir.empty()) {
            // 表示路径本身就是父目录，无需进一步处理
            // 但此时，path是类似"/a/b/", 所以需要确保父目录存在
            // 直接调用递归处理parent_dir
            return make_dirs(parent_dir);
        }
    }

    // 递归创建父目录
    if (parent_dir != "") {
        if (!make_dirs(parent_dir)) {
            return false;
        }
    }

    // 现在父目录已经存在，尝试创建当前目录
    if (mkdir(path.c_str(), 0777) == -1) {
        // 创建失败，返回false
        return false;
    }

    return true;
}

File::File(const String& p) : path_(p), fp_(nullptr), strong_ref_(true) { }

File::~File() { close(); }

bool File::open(OpenMode m)
{
    if (mode_) {
        NX_LOG_WARNING("open fail: %s, reason: busy\n", path_.c_str());
        return false;
    }

    fp_ = fopen(path_.c_str(), m == OpenMode::READ ? "rb" : "wb");
    if (fp_ == nullptr) {
        NX_LOG_WARNING("open fail: %s, reason: fp_ is nullptr\n",
                       path_.c_str());
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

    return IO_Success {  fread(buffer, 1, bytes, fp_) };
}

WriteResult File::write(const void* buffer, size_t bytes)
{
    if (mode_ != OpenMode::WRITE) {
        return IO_Error::NOT_OPEN;
    }

    if (ferror(fp_))
        return IO_Error::IO_FAIL;

    return IO_Success {  fwrite(buffer, 1, bytes, fp_) };
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

char get_path_separator()
{
#if NX_PLATFORM_WINDOW == NX_PLATFORM
    return '\\';
#else
    return '/';
#endif
}

String join_path(const String& p1, const String& p2)
{
    std::stringstream ss;

    char path_separator = get_path_separator();
    if (p1.size() > 0 && *(p1.end() - 1) == path_separator) {
        ss << p1.substr(0, p1.size() - 1);
    } else {
        ss << p1;
    }
    ss << path_separator;
    ss << p2;

    return ss.str();
}

ReadAllResult read_file(const String& path)
{
    File file(path);
    if (!file.open_read()) {
        return IO_Error::NOT_OPEN;
    }

    return file.read_all();
}

Vector<String> list_dir(const String& path)
{
    Vector<String> result;
    if (!exists(path) || !is_directory(path)) {
        return result;
    }
    try {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            result.push_back(entry.path().u8string());
        }
    } catch (...) {
    }
    return result;
}

String relative_path(const String& path, const String& base)
{
    return std::filesystem::relative(path, base).u8string();
}

} // namespace nx::file_system