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
    const char* name;
    ArgumentType type;
};

struct PositionalArgument {
    const char* name;
    ArgumentType type;
};

using PositionalArgumentList = Vector<PositionalArgument>;
using OptionalArgumentList = Vector<OptionalArgument>;

struct ProgramArgument;

using Handler = Function<int(const ProgramArgument*)>;

struct ProgramArgument {
    PositionalArgumentList positional_arguments;
    OptionalArgumentList optional_arguments;
    Handler handler;
    // Map<String, ProgramArgument*> sub_commands;
};

class NX_API CmdParser {
public:
    CmdParser(ProgramArgument&& argument);
    ~CmdParser();
    int handle_cmd(int argc, const char* const argv[]);

private:
    ProgramArgument argument_;
    Map<String, ArgumentValue> arg_values_;
};

class NX_API CmdParserBuilder {
public:
    CmdParserBuilder();
    ~CmdParserBuilder();
    CmdParserBuilder& add_argument(const char* name, ArgumentType type);
    CmdParserBuilder& set_handler(Handler);
    UniquePtr<CmdParser> build();

private:
    ProgramArgument argument_;
};

} // namespace nx::cmd

#define NX_MAIN(args_name, program_argument_builder)                           \
    int main(int argc, const char* const argv[])                               \
    {                                                                          \
        nx::cmd::CmdParserBuilder args_name;                                   \
        { program_argument_builder };                                          \
        return args_name.build()->handle_cmd(argc, argv);                      \
    }

// NX_MAIN({

// })