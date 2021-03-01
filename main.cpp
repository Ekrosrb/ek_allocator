
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <cassert>


#define u8 uint8_t
#define STACK_SIZE 32
#define HEAP_SIZE (STACK_SIZE * 8)

typedef struct virtual_memory
{
    u8 stack[STACK_SIZE];
    char** unmapped;
    void* heap[HEAP_SIZE];

    struct
    {
        char** data;
        char** bss;
        char* text;
    }data_t;
}virtual_memory_t;

typedef struct header
{
    u8 status;
    size_t size;
    size_t prev;
}header_t;

#define HEADER sizeof(header_t);

void* ptr;

void LOG()
{
    auto* h = static_cast<header_t *>(ptr);
    printf("OUR LIST\n");
    for (;;)
    {
        printf("Data + HEADER. [%x]. Memory of heap [%u]. Status: [%s]\n", h, h->size, ((h->status == 0)?"free":"occupied"));
        if(h->prev == 0){
            break;
        }
        h = h - h->prev;
    }

}

void create(){
    static virtual_memory_t vm;
    ptr = vm.heap;
    header_t h;
    h.status = 0;
    h.size = HEAP_SIZE;
    h.prev = 0;
    *((header_t*)ptr) = h;
    LOG();

}

header_t* new_entity(size_t size)
{
    auto* best = static_cast<header_t *>(ptr);
    if(best->prev == 0){
        return best;
    }
    header_t* prev = best - best->prev;
    for(;;){
        if(prev->status == 0 && prev->size >= size && prev->size < best->size){
            best = prev;
        }
        if(prev->prev == 0){
            break;
        }else{
            prev = prev - prev->prev;
        }
    }
    return best;
}

void* mem_alloc(size_t size)
{
    assert(size <= HEAP_SIZE);

    size += HEADER;
    header_t* h = new_entity(size);

    void* user_ptr = h + HEADER;

    size_t last = h->size - size;
    size_t head = HEADER;
    if(last <= head){
        size = h->size;

    }
    header_t* next = h+size;
    if(next->status != 1) {
        next->prev = size;
        next->status = 0;
        next->size = last;
        if (next != ptr && h != ptr) {
            header_t *n = next + next->size;
            n->prev = next->size;
        }
        if (h == ptr) {
            ptr = next;
        }
    }
    h->status = 1;
    h->size = size;
    LOG();
    return user_ptr;

}


void mem_free(void* pVoid)
{
    header_t * t = (header_t*)pVoid - HEADER
    t->status = 0;

    header_t* next = nullptr;
    header_t* prev = nullptr;
    if(t->prev != 0){
        prev = t - t->prev;
    }
    if(t != ptr){
        next = t + t->size;
    }

    if(next != nullptr && next->status == 0){
        t->size += next->size;
        if(next != ptr){
            header_t* h = next + next->size;
            h->prev += t->size;
        }else{
            ptr = t;
            next = t;
        }

    }

    if(prev != nullptr && prev->status == 0){
        prev->size += t->size;
        if(next != nullptr && next != t){
            next->prev = prev->size;
        }else{
            ptr = prev;
        }
    }


    LOG();
}

void test()
{
    printf("Header size [%u]", sizeof(header_t));
    create();
    struct foo{
        int a;
        int b;
    };

    foo* f = static_cast<struct foo *>(mem_alloc(60));
    f->a = 10;
    f->b = 15;
    foo* f1 = static_cast<struct foo *>(mem_alloc(sizeof(f)));
    f1->a = 5;
    f1->b = 7;
    foo* f2 = static_cast<struct foo *>(mem_alloc(60));
    f2->a = 5;
    f2->b = 7;
    int* a = static_cast<int *>(mem_alloc(sizeof(int)));
    mem_free(f);
    mem_free(f1);
    mem_free(a);
    mem_free(f2);


}

int main() {
    test();

    return 0;
}
