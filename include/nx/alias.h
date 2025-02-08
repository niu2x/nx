#pragma once

#include <nx/file_system.h>
#include <nx/digest.h>

/**
 * @brief root namespace
 */
namespace nx {
namespace fs = nx::file_system;
}

namespace nx {
namespace CRC32 = nx::digest::CRC32;
namespace MD5_Context = nx::digest::MD5_Context;
} // namespace nx