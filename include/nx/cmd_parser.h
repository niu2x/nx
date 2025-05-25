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

    CmdParserBuilder& set_handler(Handler);
    UniquePtr<CmdParser> build();

private:
    ProgramArgument argument_;
};

class NX_API Cmd {
public:
    Cmd();
    virtual ~Cmd();
    virtual int handle_cmd(int argc, const char* const argv[]) = 0;
};

class NX_API SingleCmd : public Cmd {
public:
    SingleCmd(UniquePtr<CmdParser> parser);
    ~SingleCmd();
    int handle_cmd(int argc, const char* const argv[]) override;

private:
    UniquePtr<CmdParser> parser_;
};

class NX_API GroupCmd : public Cmd {
public:
    GroupCmd();
    ~GroupCmd();

    int handle_cmd(int argc, const char* const argv[]) override;
    void add_sub_command(const String& name, UniquePtr<Cmd> cmd);

private:
    Map<String, UniquePtr<Cmd>> sub_commands_;
};

} // namespace nx::cmd
