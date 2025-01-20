#include <nx/alias.h>

int main()
{
    nx::fs::File file("./test.txt");

    if (file.open_write()) {
        file.write("hello world!", 12);
    } else {
        NX_PANIC("open file fail.");
    }

    return 0;
}