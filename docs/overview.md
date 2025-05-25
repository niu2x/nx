# 项目概览

<!-- - [基本类型](\ref docs/basic_type.md) -->
# basic type
- [ByteBuffer](\ref nx::ByteBuffer)
- [String](\ref nx::String)
- [Optional<T>](\ref nx::Optional)
- [Vector<T>](\ref nx::Vector)
- [SharedPtr<T>](\ref nx::SharedPtr)
- [UniquePtr<T>](\ref nx::UniquePtr)
- [Variant<T ...>](\ref nx::Variant)

<!-- # basic helper class
- [Uncopyable](\ref nx::Uncopyable) -->

# basic interface
- [Read](\ref nx::Read)
- [Write](\ref nx::Write)

# Time Utils
- [TimePoint](\ref nx::TimePoint)
- [TimeDuration](\ref nx::TimeDuration)
- [time_now](\ref nx::time_now)
- [time_diff](\ref nx::time_diff)

### Examples

```
TimePoint before = time_now();
// do something
TimePoint after = time_now();
TimeDuration duration = time_diff(before, after);
```

# Math Utils
- [ceil_pow2](\ref nx::ceil_pow2)

# File System 
- [is_directory](\ref nx::file_system::is_directory)
- [is_file](\ref nx::file_system::is_file)
- [exists](\ref nx::file_system::exists)
- [make_dirs](\ref nx::file_system::make_dirs)
- [relative_path](\ref nx::file_system::relative_path)
- [list_dir](\ref nx::file_system::list_dir)
- [get_parent_path](\ref nx::file_system::get_parent_path)
- [get_file_name](\ref nx::file_system::get_file_name)
- [join_path](\ref nx::file_system::join_path)
- [get_path_separator](\ref nx::file_system::get_path_separator)
- [File](\ref nx::file_system::File)

# Command Line Parser
```
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
```