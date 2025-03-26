#include <nx/alias.h>

int main()
{
    auto archive = nx::fs::create_archive(
        "zip:///home/niu2x/project/ant/3rd/Effekseer/docs/QuickTutorial_Tool/"
        "01_AndrewFM%20Tutorial.zip");
    printf("archive %p\n", archive.get());
    auto items = archive->list_dir("/01_AndrewFM Tutorial");
    for (auto& x : items) {
        printf("%s\n", x.c_str());
    }
    return 0;
}