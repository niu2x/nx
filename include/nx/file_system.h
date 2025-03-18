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
bool is_directory(const String& path);

/**
 * @brief      Determines whether the specified path is file.
 *
 * @param[in]  path  The path
 *
 * @return     True if the specified path is file, False otherwise.
 */
bool is_file(const String& path);

/**
 * @brief      Determines whether the specified file exist
 *
 * @param[in]  path  The path of file
 *
 * @return     True if the specified file exist, False otherwise.
 */
bool exists(const String& path);

bool make_dirs(const String& path);

class File : public Read, public Write, private Uncopyable {
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
File& in();

/**
 * @brief      get stdout file
 *
 * @return     stdout file
 */
File& out();

/**
 * @brief      get stderr file
 *
 * @return     stderr file
 */
File& err();

/**
 * @brief      Gets the file name.
 *
 * @param[in]  path  The path
 *
 * @return     The file name.
 */
String get_file_name(const String& path);

/**
 * @brief      Gets the parent path.
 *
 * @param[in]  path  The path
 *
 * @return     The parent path.
 */
String get_parent_path(const String& path);

String join_path(const String& dir, const String& path);

char get_path_separator();

ReadAllResult read_file(const String& path);
}
