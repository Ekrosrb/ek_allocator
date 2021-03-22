cmake_minimum_required(VERSION 3.17)
project(OOP1Test)

set(CMAKE_CXX_STANDARD 20)

add_executable(OOP1Test main.cpp allocator.cpp allocator.h tree.cpp tree.h core_alloc.cpp core_alloc.h)