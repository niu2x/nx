#include <nx/alias.h>

int main()
{
    auto& in = nx::fs::in();
    auto& output = nx::fs::out();
    nx::pipe(in, output);

    return 0;
}