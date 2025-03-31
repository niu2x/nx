#pragma once

#include <nx/type.h>

/**
 * @brief file_system namespace
 */
namespace nx::file_system {

/**
 * @brief      Determines whether the specified path is directory.
 *
 * @param[in]  path  The path
 *
 * @return     True if the specified path is directory, False otherwise.
 */
NX_API bool is_directory(const String& path);

/**
 * @brief      Determines whether the specified path is file.
 *
 * @param[in]  path  The path
 *
 * @return     True if the specified path is file, False otherwise.
 */
NX_API bool is_file(const String& path);

/**
 * @brief      Determines whether the specified file exist
 *
 * @param[in]  path  The path of file
 *
 * @return     True if the specified file exist, False otherwise.
 */
NX_API bool exists(const String& path);

NX_API bool make_dirs(const String& path);

NX_API String relative_path(const String& path, const String& base);

NX_API Vector<String> list_dir(const String& path);

class NX_API File : public Read, public Write, private Uncopyable {
public:
    explicit File(const String& p);

    File(const String& p, FILE* file, OpenMode m);
    ~File();

    static File& in();
    static File& out();
    static File& err();

    void close();

    bool open(OpenMode mode);
    bool open_read() { return open(OpenMode::READ); }
    bool open_write() { return open(OpenMode::WRITE); }

    const Optional<OpenMode>& get_mode() const { return mode_; }

    ReadResult read(void* buffer, size_t bytes) override;
    WriteResult write(const void* buffer, size_t bytes) override;

private:
    String path_;
    Optional<OpenMode> mode_;
    FILE* fp_;
    bool strong_ref_;
};

/**
 * @brief      get stdin file
 *
 * @return     stdin file
 */

NX_API File& in();

/**
 * @brief      get stdout file
 *
 * @return     stdout file
 */
NX_API File& out();

/**
 * @brief      get stderr file
 *
 * @return     stderr file
 */
NX_API File& err();

/**
 * @brief      Gets the file name.
 *
 * @param[in]  path  The path
 *
 * @return     The file name.
 */
NX_API String get_file_name(const String& path);

/**
 * @brief      Gets the parent path.
 *
 * @param[in]  path  The path
 *
 * @return     The parent path.
 */
NX_API String get_parent_path(const String& path);

NX_API String join_path(const String& dir, const String& path);

NX_API char get_path_separator();

NX_API ReadAllResult read_file(const String& path);

class NX_API Archive {
public:
    virtual ~Archive() = 0;
    virtual Vector<String> list_dir(const String& path) = 0;
    virtual UniquePtr<Read> open(const String& path) = 0;
};

NX_API UniquePtr<Archive> create_archive(const String& file_uri);
NX_API UniquePtr<Archive> create_zip_archive_from_memory(const void* buf,
                                                         size_t len);
}
