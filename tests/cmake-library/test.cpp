#include <nx/alias.h>

int main()
{
    auto& in = nx::fs::in();
    auto& output = nx::fs::out();
    nx::pipe(in, output);

    printf("ceil_pow2(5) == %d\n", nx::ceil_pow2(5u));
    printf("ceil_pow2(15) == %d\n", nx::ceil_pow2(15u));
    printf("ceil_pow2(16) == %d\n", nx::ceil_pow2(16u));
    printf("ceil_pow2(17) == %d\n", nx::ceil_pow2(17u));

    return 0;
}