#include <nx/alias.h>

int main()
{
    // auto archive = nx::fs::create_archive(
    //     "zip:///home/niu2x/project/llvm-project/lldb/unittests/Host/common/"
    //     "Inputs/zip-test.zip");
    // auto file = archive->open("/lib/arm64-v8a/libzip-test.so");
    // nx::pipe(*file, nx::fs::out());
    //
    //
    //
    //
    int x = 1;
    NX_LOG_DEBUG("info");
    NX_LOG_INFO("info");
    NX_LOG_WARNING("info");
    NX_LOG_ERROR("info");
    NX_LOG_CRITICAL("info");

    return 0;
}