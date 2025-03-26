#include <nx/alias.h>

int main()
{
    auto archive = nx::fs::create_archive("dir:///home/niu2x");
    auto items = archive->list_dir("xx");
    for (auto& x : items) {
        printf("%s\n", x.c_str());
    }
    return 0;
}