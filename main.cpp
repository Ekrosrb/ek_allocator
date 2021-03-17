
#include "allocator.h"


void test()
{
    struct foo{
        int a;
        int b;
    };
    foo* f = static_cast<struct foo *>(mem_alloc(20000));
    mem_show();
    void* k = mem_alloc(50);
    mem_show();
    f->a = 10;
    f->b = 15;
    f = static_cast<struct foo *>(mem_realloc(f, 25000));
    mem_show();
    mem_free(k);
    mem_show();
    foo* f2 = static_cast<struct foo *>(mem_alloc(MIN_AREA_SIZE));
    f2->a = 50;
    f2->b = 155;
    mem_show();
    mem_free(f);
    mem_show();
    mem_free(f2);
    mem_show();
}

int main() {
    test();
    return 0;
}
