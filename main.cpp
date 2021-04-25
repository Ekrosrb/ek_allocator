#include <ctime>
#include <cstdio>
#include "allocator.h"
#include <random>

void own_test(){
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
    printf("OWN TEST (WITH MEM_SHOW)");
    printf("\nstatus [0] - free, [1] - occupied\n");
    printf("Testing...\n");
    own_test();
    printf("Test successfully completed!\n");
}

using namespace std;


struct Result {
    void *ptr;
    size_t size;
    unsigned int checksum;
    bool isFree = true;
};


void buf_fill(void* ptr, size_t size){
    for(int i = 0; i < size; i++){
        *((u8*)ptr+i) = rand() % 10 + 1;
    }
}

int buf_checksum(void* ptr, size_t size){
    int sum = 0;
    for(int i = 0; i < size; i++){
        sum += *((u8*)ptr+i);
    }
    return sum;
}

int tester(int range){
    cout << "Testing..." << endl;
    const int N = 1000;
    size_t size = 0;
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(1,range);
    Result result[N] = {};
    for (int i = 0; i < N; ++i) {
        int j = rand() % N;
        if (result[j].isFree) {
            size = distribution(generator);
            result[j].ptr = mem_alloc(size);
            if (result[j].ptr != nullptr) {
                buf_fill(result[j].ptr, size);
                result[j].size = size;
                result[j].checksum = buf_checksum(result[j].ptr, size);
                result[j].isFree = false;
            }
        } else {
            if (result[j].checksum != buf_checksum(result[j].ptr, result[j].size)) {
                cout << "Checksum failed!\n";
                return -1;
            }
            if (rand() & 1) {
                size = distribution(generator);
                result[j].ptr = mem_realloc(result[j].ptr, size);
                if (result[j].ptr != nullptr) {
                    buf_fill(result[j].ptr, size);
                    result[j].size = size;
                    result[j].checksum = buf_checksum(result[j].ptr, size);
                }
            } else {
                mem_free(result[j].ptr);
                result[j].isFree = true;
            }
        }
    }
    for (auto &r: result) {
        if (!r.isFree) {
            if (r.checksum != buf_checksum(r.ptr, r.size)) {
                cout << "Checksum failed!\n";
                return -1;
            }
            mem_free(r.ptr);
        }

    }
    cout << "Auto test successfully completed!" << endl;
    return 0;
}

int main()
{

    cout << "With small data size:\n" << endl;
    tester(1000); // auto test with small data size
    cout << "\nWith big data size:\n" << endl;
    tester(1000000); // auto test with big data size

    test(); //own test

    return 0;
}