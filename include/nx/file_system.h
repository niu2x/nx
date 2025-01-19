#pragma once

#include <nx/type.h>

namespace nx::file_system {

bool is_directory(const String& path);

bool is_file(const String& path);

bool exists(const String& path);
}
