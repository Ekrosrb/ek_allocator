
#include "allocator.h"

void auto_test(){
    printf("AUTO TEST\n");
    struct foo{
        int a;
        int b;
    };
    const int DATA_SIZE = 10;
    foo* ptr[DATA_SIZE];
    int size = sizeof(foo);
    //выдиляем блоки памяти
    for(auto & i : ptr){
        i = (foo*)mem_alloc(size);
        i->a = size;
        i->b = size;
        size*=2;
    }
    printf("MEM_ALLOC  ");
    mem_show();

    printf("Foo data before realloc:\n a [");
    for(auto & i: ptr){
        printf("%u ", i->a);
    }
    printf("]\n b [");
    for(auto & i: ptr){
        printf("%u ", i->b);
    }
    printf("]\n\n");

    /* Увеличение блока выделенной памяти в 4 раза.
     * Для трех последних блоков - памяти в арене недостаточно, по этому создаються новые арены.*/
    size = sizeof(foo);
    for(auto & i : ptr){
        size *= 4;
        i = (foo*)mem_realloc(i, size);
    }

    printf("Foo data after realloc:\n a [");
    for(auto & i: ptr){
        printf("%u ", i->a);
    }
    printf("]\n b [");
    for(auto & i: ptr){
        printf("%u ", i->b);
    }
    printf("]\n\n\n");
    printf("MEM_REALLOC  ");
    mem_show();


    //освобождение памяти
    for(auto & i : ptr){
        mem_free(i);
    }
    printf("MEM_FREE  ");
    //пустой вывод mem_show() - вся память в аренах освобождена, по этому она возвращаеться ядру.
    mem_show();
}

void test()
{
    printf("\nstatus [0] - free, [1] - occupied\n\n");
    auto_test();

    printf("\n\nMANUAL TESTING\n");
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
    foo* f2 = static_cast<struct foo *>(mem_alloc(MIN_AREA_SIZE*4-200));
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
