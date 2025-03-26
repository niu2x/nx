#include <nx/file_system.h>
#include "url-parser/url.hpp"
#include "log.h"
#include <zip.h>

namespace nx::file_system {

struct FileNode {
    String name;
    Vector<UniquePtr<FileNode>> children;
};

class VFS {
public:
    VFS() { }
    ~VFS() { }
    void add_file(const String& p)
    {
        if (p.size() > 0 && p[0] == '/') {
            add_file(&root_, &p[1]);
        }
    }

    Vector<String> list_dir(const String& path) const
    {
        NX_ASSERT(path.size() > 0 && path[0] == '/',
                  "list_dir expect path start with '/'");

        Vector<String> result;
        const FileNode* node = find_node(&root_, &path[1]);
        if (node) {
            std::transform(node->children.begin(),
                           node->children.end(),
                           std::back_insert_iterator<Vector<String>>(result),
                           [](auto& x) { return x->name; });
        }
        return result;
    }

private:
    FileNode root_;

    void add_file(FileNode* file_node, const char* path)
    {
        const char* sep = strchr(path, '/');
        int name_len = 0;
        if (sep) {
            name_len = sep - path;
        } else {
            name_len = strlen(path);
        }

        String name { path, path + name_len };

        for (auto it = file_node->children.begin();
             it != file_node->children.end();
             it++) {
            if (name == (*it)->name) {
                if (sep) {
                    add_file((*it).get(), sep + 1);
                }
                return;
            }
        }

        auto new_node = std::make_unique<FileNode>();
        new_node->name = std::move(name);

        if (sep) {
            add_file(new_node.get(), sep + 1);
        }
        file_node->children.push_back(std::move(new_node));
    }

    const FileNode* find_node(const FileNode* file_node, const char* path) const
    {
        const char* sep = strchr(path, '/');
        int name_len = 0;
        if (sep) {
            name_len = sep - path;
        } else {
            name_len = strlen(path);
        }

        if (name_len == 0)
            return file_node;

        String name { path, path + name_len };

        for (auto it = file_node->children.begin();
             it != file_node->children.end();
             it++) {
            if (name == (*it)->name) {
                if (sep) {
                    return find_node((*it).get(), sep + 1);
                }
                return (*it).get();
            }
        }

        return nullptr;
    }
};

}; // namespace nx::file_system

namespace nx::file_system {
// DirArchive

class DirArchive : public Archive {
public:
    DirArchive(const String& root_dir) : root_dir_(root_dir) { }
    ~DirArchive() { }

    Vector<String> list_dir(const String& path) override
    {
        NX_ASSERT(path.size() > 0 && path[0] == '/',
                  "list_dir expect path start with '/'");

        auto dir = join_path(root_dir_, path.substr(1));
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
    ZipArchive(const String& zip_path)
    : zip_path_(zip_path)
    , zip_file_(nullptr)
    , entries_inited_(false)
    {
        int error = 0;
        zip_file_ = zip_open(
            zip_path_.c_str(), ZIP_CHECKCONS | ZIP_RDONLY, &error);
    }
    ~ZipArchive()
    {
        if (zip_file_) {
            zip_close(zip_file_);
        }
    }

    Vector<String> list_dir(const String& path) override
    {
        if (!entries_inited_) {
            entries_inited_ = true;
            init_entries();
        }

        NX_ASSERT(path.size() > 0 && path[0] == '/',
                  "list_dir expect path start with '/'");

        // if(path == "."){
        //     return vfs_.list_dir("/");
        // }
        // else if (path.substr(0, 2) == "./") {
        //     return vfs_.list_dir(path.substr(1));
        // } else if (path.size() > 0 && path[0] == '/') {
        return vfs_.list_dir(path);
        // } else {
        //     return vfs_.list_dir("/" + path);
        // }
    }
    UniquePtr<Read> open(const String& path) override
    {
        (void)path;
        if (!zip_file_)
            return nullptr;
        return nullptr;
    }

private:
    String zip_path_;
    zip_t* zip_file_;

    bool entries_inited_;
    // Vector<String> entries_;

    void init_entries()
    {
        if (!zip_file_)
            return;

        auto num_entries = zip_get_num_entries(zip_file_, 0);
        while (num_entries-- > 0) {
            String name = zip_get_name(zip_file_, num_entries, 0);
            if (name.size() > 0) {
                if (name[name.size() - 1] != '/') {
                    // entries_.emplace_back(name);
                    vfs_.add_file("/" + name);
                }
            }
        }
        // std::sort(entries_.begin(), entries_.end());
    }

    VFS vfs_;
};

} // namespace nx::file_system

namespace nx::file_system {

Archive::~Archive() { }

UniquePtr<Archive> create_archive(const String& file_uri)
{
    try {
        Url u1(file_uri);
        auto scheme = u1.scheme();
        printf("xx %s\n", scheme.c_str());
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
        error_log("url parse error: %s\n", file_uri.c_str());
        return nullptr;
    }
}

} // namespace nx::file_system