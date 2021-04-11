#include "stdio.h"
#include "allocator.h"


void auto_test(){
    printf("AUTO TEST\n");
    const int DATA_SIZE = 25;
    u8* ptr[DATA_SIZE];
    int size = sizeof(u8);

    for(auto & i : ptr){
        i = (u8*)mem_alloc(size);
        //заполняем память данными
        for(int a = 0; a < size; a++){
            *(i+a) = 1;
        }
        size*=2;
    }
    mem_show();

    size = sizeof(u8)*2;
    for(int i = 0; i < DATA_SIZE; i++){
        size *= 2;
        if(i%2 == 0) {
            ptr[i] = (u8 *) mem_realloc(ptr[i], size);
        }else{
            mem_free(ptr[i]);
        }
    }
    mem_show();
    size = sizeof(u8)*2;
    for(int i = 0; i < DATA_SIZE; i++){
        size *= 2;
        if(i%2 == 0){
            mem_free(ptr[i]);
        }else{
            ptr[i] = (u8*)mem_alloc(size);
        }
    }
    mem_show();
    size = sizeof(u8)*2;
    for(int i = 0; i < DATA_SIZE; i++){
        size *= 2;
        if(i%2 == 0){
            ptr[i] = (u8*)mem_alloc(size);
        }
    }
    mem_show();
    //освобождение памяти
    for(auto & i : ptr){
        mem_free(i);
    }

    printf("MEM AFTER FREE\n\n");
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

using namespace std;

int main()
{
    void* t = mem_alloc(5);
    void* a = mem_alloc(50);
    void* b = mem_alloc(500);
    void* c = mem_alloc(MIN_AREA_SIZE + 10);
    void* d = mem_alloc(SIZE_MAX);
    void* e = mem_alloc(ALLOC_MAX_SIZE);
    return 0;
}

// This code is contributed by rathbhupendra
