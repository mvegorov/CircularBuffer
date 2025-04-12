cmake_minimum_required(VERSION 3.17.5)
project(lab8)

set(CMAKE_CXX_STANDARD 17)

link_directories(lib)

add_subdirectory(lib)
add_subdirectory(bin)

enable_testing()
add_subdirectory(tests)

