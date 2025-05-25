#include <nx/cmd_parser.h>
#include <iostream>
#include <nx/file_system.h>

#if NX_PLATFORM_WINDOW == NX_PLATFORM
    #define strcasecmp _stricmp
#endif

namespace nx::cmd {

Optional<int> string_to_int(const char* cstr)
{
    char* endptr;
    long result = std::strtol(cstr, &endptr, 10); // 转换为整数，基数为10

    // 检查是否完全转换成功
    if (*endptr != '\0') {
        return std::nullopt;
    }

    return static_cast<int>(result);
}

Optional<bool> string_to_bool(const char* cstr)
{
    if (strcasecmp(cstr, "true") == 0 || strcasecmp(cstr, "t") == 0
        || strcasecmp(cstr, "1") == 0)
        return true;

    if (strcasecmp(cstr, "false") == 0 || strcasecmp(cstr, "f") == 0
        || strcasecmp(cstr, "0") == 0)
        return false;

    return std::nullopt;
}

Optional<ArgumentValue> parse_value(ArgumentType type, const char* x)
{
    switch (type) {
        case ArgumentType::INT: {
            return string_to_int(x);
        }
        case ArgumentType::STRING: {
            return String(x);
        }
        case ArgumentType::BOOLEAN: {
            return string_to_bool(x);
        }
    }
    return std::nullopt;
}

CmdParser::CmdParser(ProgramArgument&& argument)
: argument_(std::move(argument))
{
}

CmdParser::~CmdParser() { }

int CmdParser::handle_cmd(int argc, const char* const argv[])
{
    size_t pos_arg_index = 0;
    for (int i = 1; i < argc; i++) {
        auto value = argv[i];
        if (strcmp(value, "--help") == 0) {
            show_usage(argv[0]);
            return 1;
        } else if (value[0] != '-') {
            if (argument_.positional_arguments.size() > pos_arg_index) {
                auto argument = argument_.positional_arguments[pos_arg_index++];
                auto optional_v = parse_value(argument.type, value);
                if (optional_v) {
                    arg_values_[argument.name] = std::move(*optional_v);
                } else {
                    std::cerr << "invalid value for " << argument.name
                              << std::endl;
                    return 1;
                }
            } else {
                std::cerr << "too many arguments " << value << std::endl;
                return 1;
            }
        } else if (strlen(value) > 2 && value[0] == '-' && value[1] == '-') {
            auto name = value + 2;
            auto it = argument_.optional_arguments.find(name);
            if (it != argument_.optional_arguments.end()) {
                auto val = argv[++i];
                if (val) {

                    auto optional_v = parse_value(it->second.type, val);
                    if (optional_v) {
                        arg_values_[name] = std::move(*optional_v);
                    } else {
                        std::cerr << "invalid value for " << name << std::endl;
                        return 1;
                    }
                } else {
                    std::cerr << "no value for " << value << std::endl;
                    return 1;
                }
            } else {
                std::cerr << "invalid argument " << value << std::endl;
                return 1;
            }
        } else {
            std::cerr << "invalid argument " << value << std::endl;
            return 1;
        }
    }

    for (auto& arg : argument_.positional_arguments) {
        if (arg_values_.find(arg.name) == arg_values_.end()) {
            std::cerr << "expect argument " << arg.name << std::endl;
            return 1;
        }
    }

    for (auto& arg : argument_.optional_arguments) {
        if (arg_values_.find(arg.first) == arg_values_.end()) {
            arg_values_[arg.first] = arg.second.default_value;
        }
    }

    if (argument_.handler)
        argument_.handler(this);

    return 0;
}

void CmdParser::show_usage(const char* program_name)
{
    std::cerr << " Usage: " << file_system::get_file_name(program_name);
    std::cerr << " [OPTIONS]";
    for (auto& item : argument_.positional_arguments) {
        std::cerr << " "
                  << "<" << item.name << ">";
    }
    std::cerr << std::endl;
    std::cerr << "  Options:" << std::endl;
    for (auto& item : argument_.optional_arguments) {
        std::cerr << "  "
                  << "--" << item.first << std::endl;
    }
}

CmdParserBuilder::CmdParserBuilder() { }
CmdParserBuilder::~CmdParserBuilder() { }

CmdParserBuilder& CmdParserBuilder::add_argument(const char* name,
                                                 ArgumentType type)
{
    argument_.positional_arguments.push_back({ name, type });
    return *this;
}

CmdParserBuilder& CmdParserBuilder::add_argument(const char* name,
                                                 ArgumentType type,
                                                 ArgumentValue default_value)
{

    argument_.optional_arguments[name] = { name,
                                           type,
                                           std::move(default_value) };
    return *this;
}

CmdParserBuilder& CmdParserBuilder::set_handler(Handler h)
{
    argument_.handler = h;
    return *this;
}

UniquePtr<CmdParser> CmdParserBuilder::build()
{
    return std::make_unique<CmdParser>(std::move(argument_));
}

Cmd::Cmd() { }
Cmd::~Cmd() { }

SingleCmd::SingleCmd(UniquePtr<CmdParser> parser) : parser_(std::move(parser))
{
}

SingleCmd::~SingleCmd() { }

int SingleCmd::handle_cmd(int argc, const char* const argv[])
{
    return parser_->handle_cmd(argc, argv);
}

GroupCmd::GroupCmd() { }
GroupCmd::~GroupCmd() { }

void GroupCmd::add_sub_command(const String& name, UniquePtr<Cmd> cmd)
{
    sub_commands_[name] = std::move(cmd);
}

int GroupCmd::handle_cmd(int argc, const char* const argv[])
{
    if (argc >= 2) {
        String cmd = argv[1];
        if (cmd == "--help" || cmd == "help") {
            std::cerr << " Usage: " << file_system::get_file_name(argv[0]);
            std::cerr << " [SUB COMMANDS]" << std::endl;
            for (auto& item : sub_commands_) {
                std::cerr << " " << item.first << std::endl;
            }
        } else {
            auto it = sub_commands_.find(cmd);
            if (it != sub_commands_.end()) {
                std::vector<const char*> sub_argv;
                String program_name = argv[0];
                program_name += " ";
                program_name += argv[1];

                sub_argv.push_back(program_name.c_str());

                for (int i = 2; i < argc; i++) {
                    sub_argv.push_back(argv[i]);
                }
                return it->second->handle_cmd(argc - 1, sub_argv.data());
            } else {
                std::cerr << "no such sub_command: " << cmd << std::endl;
            }
        }
    } else {
        std::cerr << "expect a sub_command" << std::endl;
    }
    return 1;
}

} // namespace nx::cmd