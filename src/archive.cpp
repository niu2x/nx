#include <nx/file_system.h>
#include "url-parser/url.hpp"
#include <zip.h>

namespace nx::file_system {
// DirArchive

class DirArchive : public Archive {
public:
    DirArchive(const String& root_dir) : root_dir_(root_dir) { }
    ~DirArchive() { }

    Vector<String> list_dir(const String& path) override
    {
        auto dir = join_path(root_dir_, path);
        auto result = file_system::list_dir(dir);

        for (auto& x : result) {
            x = relative_path(x, dir);
        }
        return result;
    }
    UniquePtr<Read> open(const String& path) override
    {
        auto file_path = join_path(root_dir_, path);
        auto file = std::make_unique<File>(file_path);
        if (file->open_read())
            return file;
        return nullptr;
    }

private:
    String root_dir_;
};

class ZipArchive : public Archive {
public:
    ZipArchive(const String& zip_path) : zip_path_(zip_path), zip_file_(nullptr)
    {
        int error = 0;
        zip_file_ = zip_open(
            zip_path_.c_str(), ZIP_CHECKCONS | ZIP_RDONLY, &error);
        init_entries();
    }
    ~ZipArchive()
    {
        if (zip_file_) {
            zip_close(zip_file_);
        }
    }

    Vector<String> list_dir(const String& path) override
    {
        if (!zip_file_)
            return {};
        return {};
    }
    UniquePtr<Read> open(const String& path) override
    {
        if (!zip_file_)
            return nullptr;
        return nullptr;
    }

private:
    String zip_path_;
    zip_t* zip_file_;

    void init_entries()
    {
        if (!zip_file_)
            return;
        zip_get_num_entries(zip_file_, 0);
    }
};

} // namespace nx::file_system

namespace nx::file_system {

Archive::~Archive() { }

UniquePtr<Archive> create_archive(const String& file_uri)
{
    try {
        Url u1(file_uri);
        auto scheme = u1.scheme();
        auto path = u1.path();
        if (scheme == "dir") {
            return std::make_unique<DirArchive>(path);
        } else if (scheme == "zip") {
#if defined(USE_LIBZIP)
            return std::make_unique<ZipArchive>(path);
#else
            return nullptr;
#endif
        } else {
            return nullptr;
        }
    } catch (...) {
        return nullptr;
    }
}

} // namespace nx::file_system