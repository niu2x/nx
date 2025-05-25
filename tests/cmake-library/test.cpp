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

// ("age", INT)
// ("sex", BOOLEAN, true)

int main(int argc, const char* const argv[])
{
    nx::cmd::CmdParserBuilder args_1;
    args_1.add_argument("age", nx::cmd::ArgumentType::INT);
    args_1.add_argument("sex", nx::cmd::ArgumentType::BOOLEAN, true);
    args_1.set_handler([](const nx::cmd::CmdParser* args_1) {
        int age = args_1->get<int>("age");
        std::cout << "age: " << age << std::endl;
        bool sex = args_1->get<bool>("sex");
        std::cout << "sex: " << sex << std::endl;
        return 0;
    });
    auto cmd_a = args_1.build();

    nx::cmd::CmdParserBuilder args_2;
    args_2.add_argument("money", nx::cmd::ArgumentType::INT);
    args_2.set_handler([](const nx::cmd::CmdParser* args_2) {
        int money = args_2->get<int>("money");
        std::cout << "money: " << money << std::endl;
        return 0;
    });
    auto cmd_b = args_2.build();

    auto group_cmd = std::make_unique<nx::cmd::GroupCmd>();
    group_cmd->add_sub_command(
        "a", std::make_unique<nx::cmd::SingleCmd>(std::move(cmd_a)));
    group_cmd->add_sub_command(
        "b", std::make_unique<nx::cmd::SingleCmd>(std::move(cmd_b)));

    nx::cmd::GroupCmd group_cmd_a;
    group_cmd_a.add_sub_command("test", std::move(group_cmd));

    return group_cmd_a.handle_cmd(argc, argv);
}