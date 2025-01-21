#pragma once

#include <nx/type.h>

namespace nx::file_system {

bool is_directory(const String& path);

bool is_file(const String& path);

bool exists(const String& path);

class File : public Read, public Write, private Uncopyable {
public:
    explicit File(const String& p = "");
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

File& in();
File& out();
File& err();

String get_file_name(const String& path);
String get_parent_path(const String& path);
}
