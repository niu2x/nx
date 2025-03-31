#include <nx/file_system.h>
#include "url-parser/url.hpp"
#include "log.h"

#if defined(USE_LIBZIP)
    #include <zip.h>
#endif

namespace nx::file_system {

template <class Data>
struct FileNode {
    String name;
    Vector<UniquePtr<FileNode>> children;
    Data data;
};

template <class Data>
class VFS {
public:
    using FileNodeType = FileNode<Data>;
    VFS() { }
    ~VFS() { }
    void add_file(const String& path)
    {
        NX_ASSERT(path.size() > 0 && path[0] == '/',
                  "add_file expect path start with '/'");

        add_file(&root_, &path[1]);
    }

    Vector<String> list_dir(const String& path) const
    {
        NX_ASSERT(path.size() > 0 && path[0] == '/',
                  "list_dir expect path start with '/'");

        Vector<String> result;
        const FileNodeType* node = find_node(&root_, &path[1]);
        if (node) {
            std::transform(node->children.begin(),
                           node->children.end(),
                           std::back_insert_iterator<Vector<String>>(result),
                           [](auto& x) { return x->name; });
        }
        return result;
    }

    FileNodeType* find_node(const String& path)
    {
        NX_ASSERT(path.size() > 0 && path[0] == '/',
                  "find_node expect path start with '/'");
        return const_cast<FileNodeType*>(find_node(&root_, &path[1]));
    }

private:
    FileNodeType root_;

    void add_file(FileNodeType* file_node, const char* path)
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

        auto new_node = std::make_unique<FileNodeType>();
        new_node->name = std::move(name);

        if (sep) {
            add_file(new_node.get(), sep + 1);
        }
        file_node->children.push_back(std::move(new_node));
    }

    const FileNodeType* find_node(const FileNodeType* file_node,
                                  const char* path) const
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
    explicit DirArchive(const String& root_dir) : root_dir_(root_dir) { }
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
        NX_ASSERT(path.size() > 0 && path[0] == '/',
                  "open expect path start with '/'");

        auto file_path = join_path(root_dir_, path);
        auto file = std::make_unique<File>(file_path);
        if (file->open_read())
            return file;
        return nullptr;
    }

private:
    String root_dir_;
};

#if defined(USE_LIBZIP)

class ZipEntry : public Read {
public:
    explicit ZipEntry(zip_file_t* entry) : entry_(entry) { }

    ~ZipEntry() { zip_fclose(entry_); }

    ReadResult read(void* buffer, size_t bytes)
    {
        auto result = zip_fread(entry_, buffer, bytes);

        if (result == 0)
            return EndOfFile {};

        if (result == -1)
            return IO_Error::IO_FAIL;

        return IO_Success { .bytes = (size_t)result };
    }

private:
    zip_file_t* entry_;
};

class ZipArchive : public Archive {
public:
    ZipArchive(const void* buf, size_t len) : zip_path_(""), zip_file_(nullptr)
    {
        zip_error_t error;
        auto source = zip_source_buffer_create(buf, len, 0, &error);

        if (source) {
            zip_file_ = zip_open_from_source(
                source, ZIP_CHECKCONS | ZIP_RDONLY, &error);
            init_entries();

            if (!zip_file_) {
                zip_source_free(source);
            }
        } else {
            zip_source_free(source);
        }
    }

    explicit ZipArchive(const String& zip_path)
    : zip_path_(zip_path)
    , zip_file_(nullptr)
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

        NX_ASSERT(path.size() > 0 && path[0] == '/',
                  "list_dir expect path start with '/'");
        return vfs_.list_dir(path);
    }

    UniquePtr<Read> open(const String& path) override
    {
        NX_ASSERT(path.size() > 0 && path[0] == '/',
                  "Archive::open expect path start with '/'");

        auto node = vfs_.find_node(path);
        if (!node)
            return nullptr;

        auto index = node->data;
        zip_file_t* file = zip_fopen_index(zip_file_, index, 0);
        if (!file)
            return nullptr;

        return std::make_unique<ZipEntry>(file);
    }

private:
    String zip_path_;
    zip_t* zip_file_;

    void init_entries()
    {
        if (!zip_file_)
            return;

        auto num_entries = zip_get_num_entries(zip_file_, 0);
        while (num_entries-- > 0) {
            String name = zip_get_name(zip_file_, num_entries, 0);
            if (name.size() > 0) {
                if (name[name.size() - 1] != '/') {
                    vfs_.add_file("/" + name);
                    auto node = vfs_.find_node("/" + name);
                    node->data = num_entries;
                }
            }
        }
    }

    VFS<zip_uint64_t> vfs_;
};

#endif

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
        error_log("url parse error: %s\n", file_uri.c_str());
        return nullptr;
    }
}

UniquePtr<Archive> create_zip_archive_from_memory(const void* buf, size_t len)
{
#if defined(USE_LIBZIP)
    return std::make_unique<ZipArchive>(buf, len);
#else
    (void) buf;
    (void) len;
    return nullptr;
#endif
}

} // namespace nx::file_system