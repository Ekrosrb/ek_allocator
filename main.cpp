#include <cstdio>
#include <cstring>
#include <cstdint>
#include <cassert>

#define u8 uint8_t
#define HEAP_SIZE (4096)

typedef struct virtual_heap
{
    u8 heap[HEAP_SIZE];
}heap_t;

typedef struct header
{
    u8 status;
    size_t size;
    size_t prev;
}header_t;

#define HEADER sizeof(header_t)

u8* ptr;

static bool init = false;

void mem_show()
{
    auto* h = reinterpret_cast<header_t *>(ptr);
    printf("OUR LIST\n");
    for (;;)
    {
        u8 status = h->status;
        printf("Data + HEADER. [%x]. Memory of heap [%u]. Status: [%s]. ", h, h->size,(!status)?"free":"occupied");
        if(!status && h->size-HEADER != 0) {
            printf("Available for selection: [%u].\n", h->size-2*HEADER);
        }else{
            printf("\n");
        }
        if(h->prev == 0){
            break;
        }
        h = reinterpret_cast<header_t *>((u8 *) h - h->prev);
    }

}

void create(){
    static heap_t vm;
    ptr = vm.heap;
    header_t h;
    h.status = 0;
    h.size = HEAP_SIZE;
    h.prev = 0;
    *((header_t*)ptr) = h;
    init = true;
}

header_t* new_entity(size_t size)
{
    if(!init){
        create();
    }
    auto* best = reinterpret_cast<header_t *>(ptr);
    if(best->prev == 0){
        return best;
    }
    auto* prev = reinterpret_cast<header_t *>((u8 *) best - best->prev);
    for(;;){
        if(prev->status == 0 && prev->size >= size && prev->size < best->size){
            best = prev;
        }
        if(prev->prev == 0){
            break;
        }else{
            prev = reinterpret_cast<header_t *>((u8 *) prev - prev->prev);
        }
    }
    return best;
}

void* mem_alloc(size_t size)
{
    assert(size <= HEAP_SIZE);
    size += HEADER;

    header_t* h = new_entity(size);
    assert(h->size >= size+sizeof(header_t));
    void* user_ptr = (u8*)h + HEADER;

    size_t last = h->size - size;
    if(last <= HEADER){
        size = h->size;
    }else if(last-HEADER <= HEADER){
        size += last-HEADER;
        last -= last-HEADER;
    }


    if((u8*)h != ptr) {
        if (size != h->size) {
            header_t next;
            next.prev = size;
            next.status = 0;
            next.size = last;
            void *next_ptr = (u8*)h + h->size;
            *((header_t *) next_ptr) = next;
        }
    }else{
        header_t next;
        next.prev = size;
        next.status = 0;
        next.size = last;
        ptr += size;
        *((header_t*)ptr) = next;

    }
    h->status = 1;
    h->size = size;
    return user_ptr;

}


void mem_free(void* pVoid)
{
    void* t = (u8*)pVoid - HEADER;
    ((header_t*)t)->status = 0;

    header_t* next = nullptr;
    header_t* prev = nullptr;
    if(((header_t*)t)->prev != 0){
        prev = reinterpret_cast<header_t *>((u8 *) t - ((header_t *) t)->prev);
    }
    if(t != ptr){
        next = reinterpret_cast<header_t *>((u8 *) t + ((header_t *) t)->size);
    }

    if(next != nullptr && next->status == 0){
        ((header_t*)t)->size += next->size;
        if((u8*)next != ptr){
            auto* h = reinterpret_cast<header_t *>((u8 *) next + next->size);
            h->prev += ((header_t*)t)->size;
        }else{
            ptr = (u8*)t;
            next = (header_t*)t;
        }

    }

    if(prev != nullptr && prev->status == 0){
        prev->size += ((header_t*)t)->size;
        if(next != nullptr && next != t){
            next->prev = prev->size;
        }else{
            ptr = (u8*)prev;
        }
    }
}

void test()
{

    struct foo{
        int a;
        int b;
    };

    foo* f = static_cast<struct foo *>(mem_alloc(3000));
    f->a = 10;
    f->b = 15;
    foo* f1 = static_cast<struct foo *>(mem_alloc(sizeof(f)));
    f1->a = 5;
    f1->b = 7;
    mem_show();
    foo* f2 = static_cast<struct foo *>(mem_alloc(60));
    f2->a = 5;
    f2->b = 7;
    mem_show();
    int* a = static_cast<int *>(mem_alloc(sizeof(int)));
    mem_show();
    mem_free(f);
    mem_show();
    mem_free(f1);
    mem_show();
    mem_free(a);
    mem_show();
    mem_free(f2);
    mem_show();


}

int main() {
    test();

    return 0;
}
