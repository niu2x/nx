#include <nx/cmd_parser.h>

namespace nx::cmd {

CmdParser::CmdParser(ProgramArgument&& argument)
: argument_(std::move(argument))
{
}

CmdParser::~CmdParser() { }

int CmdParser::handle_cmd(int argc, const char* const argv[])
{
    int pos_arg_index = 0;
    for (int i = 1; i < argv; i++) {
        auto value = argv[i];
        if (value[0] != '-') {
            if (argument_.positional_arguments.size() > pos_arg_index) {
                auto argument = argument_.positional_arguments[pos_arg_index++];
                
            } else {
                NX_PANIC("too many arguments");
            }
        } else {
        }
    }

    return 0;
}

CmdParserBuilder::CmdParserBuilder() { }
CmdParserBuilder::~CmdParserBuilder() { }

CmdParserBuilder& CmdParserBuilder::add_argument(const char* name,
                                                 ArgumentType type)
{
    argument_.positional_arguments.push_back({ name, type });
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