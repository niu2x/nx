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

using CRC32 = nx::digest::CRC32;
using MD5 = nx::digest::MD5;
using SHA256 = nx::digest::SHA256;
using nx::digest::crc32;
using nx::digest::md5;
using nx::digest::sha256;

} // namespace nx