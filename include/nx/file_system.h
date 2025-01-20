#pragma once

#include <nx/type.h>

namespace nx::file_system {

bool is_directory(const String& path);

bool is_file(const String& path);

bool exists(const String& path);

class File : public Read, public Write, private Uncopyable {

public:
    explicit File(const String& p = "");
    ~File();

    void close();

    bool open(OpenMode mode);
    bool open_read() { return open(OpenMode::READ); }
    bool open_write() { return open(OpenMode::WRITE); }

    const Optional<OpenMode>& get_mode() const { return mode_; }

    ReadResult read(void* buffer, size_t bytes) override;
    WriteResult write(const void* buffer, size_t bytes) override;

private:
    Optional<OpenMode> mode_;
    FILE* fp_;
    String path_;
};
}
