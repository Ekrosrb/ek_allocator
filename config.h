#define u8 char
#define MIN_ALLOCATED_SIZE alignof(std::max_align_t)
#define ALIGN(s) (((s) + (MIN_ALLOCATED_SIZE - 1)) & ~(MIN_ALLOCATED_SIZE - 1))
#define MIN_AREA_SIZE 1048576 //1MB
#define PAGE_SIZE 4096
#define HEADER_SIZE ALIGN(sizeof(header_t))
#define ALLOC_MAX_SIZE (SIZE_MAX-2*PAGE_SIZE)