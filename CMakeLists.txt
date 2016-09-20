cmake_minimum_required(VERSION 3.4)

set(CMAKE_C_FLAGS "-O3 -Wall")

add_subdirectory(modules/libkss) 

file(GLOB SOURCES src/*.c src/**/*.c)

include_directories(modules/libkss/src)
include_directories(modules/libkss/modules)

add_executable(kss2vgm src/kss2vgm.c)
target_link_libraries(kss2vgm kss)
