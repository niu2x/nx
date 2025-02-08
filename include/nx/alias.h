#pragma once

#include <nx/file_system.h>
#include <nx/digest.h>

/**
 * @brief root namespace
 */
namespace nx {
/**
 * alias for nx::file_system
 */
namespace fs = nx::file_system;
}

namespace nx {

namespace CRC32 = nx::digest::CRC32;
namespace MD5 = nx::digest::MD5;

} // namespace nx