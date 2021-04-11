#include "allocator.h"

//void* default_area = nullptr;
//size_t default_area_size = 0;
Node* root = nullptr;
void mem_show(){

}

void mem_free(void* pVoid){
    if(!pVoid){
        return;
    }

    header_t* h = get_h((u8*)pVoid-HEADER_SIZE);
    h->status = 0;
    header_t* next = !h->is_last ? next_h(h) : nullptr;
    header_t* prev = !h->is_first ? prev_h(h) : nullptr;

    if(next && !next->status){
        root = deleteNode(root, next->size);
        h = merge(h, next);
    }

    if(prev && !prev->status){
        root = deleteNode(root, prev->size);
        h = merge(prev, h);
    }

    if(h->is_first && h->is_last){
        core_free(h);
    }else{
        root = insert(root, h->size, h, &h->node);
    }
}

void* mem_alloc(size_t size){
    if(size > ALLOC_MAX_SIZE || size == 0){
        return nullptr;
    }
    size = ALIGN(size+HEADER_SIZE);
    header_t* h = best(size);
    if(!h) return nullptr;
    split(h, size);
    h->status = 1;
    return user_ptr(h);
}

void* mem_realloc(void* ptr, size_t size){
    if (!size) {
        mem_free(ptr);
        return nullptr;
    }
    if (!ptr) {
        return mem_alloc(size);
    }

    header_t* h = get_h((u8*)ptr-HEADER_SIZE);
    size = ALIGN(size+HEADER_SIZE);
    if(size == h->size){
        return ptr;
    }

    if(size < h->size){
        split(h, size);
        return ptr;
    }

    if(size > h->size && !h->is_last){
        header_t* next = next_h(h);
        if(!next->status && h->size + next->size >= size){
            root = deleteNode(root, next->size);
            split(merge(h, next), size);
            return ptr;
        }
    }

    void* realloc = mem_alloc(size);
    if (realloc) {
        memcpy(realloc, ptr, h->size);
        mem_free(ptr);
    }
    return realloc;
}

header_t* best(size_t size){
    Node* node = nullptr;
    if(root) {
        node = search(root, size);
    }

    if(!root || !node || !node->ptr){
        size_t s = new_area_size(size);
        void* mem = core_alloc(s);
        return !mem ? nullptr : create_header(nullptr, s, mem);
    }
    void* ptr = node->ptr;
    root = deleteNode(root, node->key);
    return (header_t*)ptr;
}
//prev
//size - size of new header
//mem
header_t* create_header(header_t* prev, size_t size, void* mem){
    header_t* h;
    if(prev){

        header_t* next = next_h(prev);
        prev->size -= size;
        h = get_h((u8*)prev + prev->size);
        h->size = size;
        h->prev = prev->size;
        h->is_first = 0;

        if(!prev->is_last){
            next->prev = h->size;
            h->is_last = 0;
        }else{
            prev->is_last = 0;
            h->is_last = 1;
        }
    }else{
        h = (header_t*)mem;
        h->prev = 0;
        h->size = size;
        h->is_last = h->is_first = 1;
    }
    h->status = 0;
    return h;
}

header_t* next_h(header_t* h){
    return get_h((u8 *) h + h->size);
}

header_t* prev_h(header_t* h){
    return get_h((u8 *) h - h->prev);
}

size_t new_area_size(size_t size){
    size_t n_size = MIN_AREA_SIZE;
    if(size > n_size){
        n_size = std::ceil((1.0*size)/PAGE_SIZE)*PAGE_SIZE;
    }
    return n_size;
}

void* user_ptr(header_t* h){
    return (u8*)h+HEADER_SIZE;
}

header_t* merge(header_t* prev, header_t* next){
    prev->size += next->size;
    if(!next->is_last){
        next_h(next)->prev = prev->size;
    }else{
        prev->is_last = 1;
    }
    return prev;
}

void split(header_t* h, size_t size){
    size_t last = ALIGN(h->size - size);

    if(last > HEADER_SIZE + MIN_ALLOCATED_SIZE){
        header_t* last_h = create_header(h, last, nullptr);
        root = insert(root, last_h->size, last_h, &last_h->node);
    }
}

header_t* get_h(void* h){
    return (header_t*)h;
};

void clear_node(Node* h){

}