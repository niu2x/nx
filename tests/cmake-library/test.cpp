#include <nx/alias.h>
#include <nx/log.h>
#include <nx/cmd_parser.h>
#include <iostream>

// NX_MAIN(args, {
//     args.add_argument("age", nx::cmd::ArgumentType::INT)
//         .set_handler([](auto* args) { return 0; });
//     // auto archive = nx::fs::create_archive(
//     // "zip:///home/niu2x/project/llvm-project/lldb/unittests/Host/common/"
//     //     "Inputs/zip-test.zip");
//     // auto file = archive->open("/lib/arm64-v8a/libzip-test.so");
//     // nx::pipe(*file, nx::fs::out());
//     //
//     //
//     //
//     //
//     // NX_LOG_DEBUG("debug");
//     // NX_LOG_INFO("info");
//     // NX_LOG_WARNING("WARNING");
//     // NX_LOG_ERROR("error");
//     // NX_LOG_CRITICAL("critical");
// })

int main(int argc, const char* const argv[])
{
    nx::cmd::CmdParserBuilder args;
    args.add_argument("age", nx::cmd::ArgumentType::INT);
    args.add_argument("sex", nx::cmd::ArgumentType::BOOLEAN, true);
    args.set_handler([](auto* args) {
        int age = std::get<int>(args->find("age")->second);
        std::cout << "age: " << age << std::endl;
        int sex = std::get<bool>(args->find("sex")->second);
        std::cout << "sex: " << sex << std::endl;
        return 0;
    });
    NX_LOG_INFO("info");
    return args.build()->handle_cmd(argc, argv);
}