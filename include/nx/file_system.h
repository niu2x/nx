#pragma once

#include <nx/type.h>

namespace nx::file_system {

bool is_directory(const String& path);

bool is_file(const String& path);

bool exists(const String& path);

class File : public Read, private Uncopyable {
public:
    explicit File(const String& p = "");
    ~File();

    void close();
    bool open(OpenMode mode);

    const Optional<OpenMode>& get_mode() const { return mode_; }

    ReadResult read(void* buffer, size_t bytes) override;

private:
    Optional<OpenMode> mode_;
    FILE* fp_;
    String path_;
};
}
