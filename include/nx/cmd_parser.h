#pragma once

#include <nx/type.h>

namespace nx::cmd {

enum ArgumentType {
    STRING,
    INT,
    BOOLEAN,
};

using ArgumentValue = Variant<String, int, bool>;

struct OptionalArgument {
    String name;
    ArgumentType type;
    ArgumentValue default_value;
};

struct PositionalArgument {
    String name;
    ArgumentType type;
};

using PositionalArgumentList = Vector<PositionalArgument>;
using OptionalArgumentMap = Map<String, OptionalArgument>;

using ArgumentValueMap = Map<String, ArgumentValue>;

using Handler = Function<int(const ArgumentValueMap*)>;

class CmdParser;

struct ProgramArgument {
    PositionalArgumentList positional_arguments;
    OptionalArgumentMap optional_arguments;
    Handler handler;
    Map<String, UniquePtr<CmdParser>> sub_commands;
};

class NX_API CmdParser {
public:
    CmdParser(ProgramArgument&& argument);
    ~CmdParser();
    int handle_cmd(int argc, const char* const argv[]);

private:
    ProgramArgument argument_;
    ArgumentValueMap arg_values_;
};

class NX_API CmdParserBuilder {
public:
    CmdParserBuilder();
    ~CmdParserBuilder();
    CmdParserBuilder& add_argument(const char* name, ArgumentType type);
    CmdParserBuilder& add_argument(const char* name,
                                   ArgumentType type,
                                   ArgumentValue default_value);
    CmdParserBuilder& add_sub_command(const char* name,
                                      UniquePtr<CmdParser> parser);
    CmdParserBuilder& set_handler(Handler);
    UniquePtr<CmdParser> build();

private:
    ProgramArgument argument_;
};

} // namespace nx::cmd
