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

class CmdParser;
using Handler = Function<int(const CmdParser*)>;

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

    template <class T>
    T get(const String& key) const
    {
        auto it = arg_values_.find(key);
        if (it == arg_values_.end()) {
            NX_PANIC("no such argument %s", key.c_str());
        }
        return std::get<T>(it->second);
    }

private:
    ProgramArgument argument_;
    ArgumentValueMap arg_values_;
    void show_usage(const char* program_name);
};

class NX_API CmdParserBuilder {
public:
    CmdParserBuilder();
    ~CmdParserBuilder();
    CmdParserBuilder& add_argument(const char* name, ArgumentType type);

    CmdParserBuilder& add_argument(const char* name,
                                   ArgumentType type,
                                   ArgumentValue default_value);

    CmdParserBuilder& add_argument(const char* name,
                                   ArgumentType type,
                                   const char* v)
    {
        return add_argument(name, type, String(v));
    }

    CmdParserBuilder& add_sub_command(const char* name,
                                      UniquePtr<CmdParser> parser);
    CmdParserBuilder& set_handler(Handler);
    UniquePtr<CmdParser> build();

private:
    ProgramArgument argument_;
};

} // namespace nx::cmd
