#include <nx/cmd_parser.h>
#include <iostream>
#include <nx/file_system.h>

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

    for (auto& item : arg_values_) {
        printf("%s \n", item.first.c_str());
        printf("%s \n", std::get<std::string>(item.second).c_str());
    }

    if (argument_.handler)
        argument_.handler(&arg_values_);

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

    argument_.optional_arguments[name] = { name, type, default_value };
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

} // namespace nx::cmd