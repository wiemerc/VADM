cmake_minimum_required(VERSION 3.5)
project(VADM)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32 -Wall -g  -I/opt/local/include -I/usr/local/include -I/opt/m68k-amigaos/os-include")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/opt/local/lib -L/usr/local/lib")

set(SOURCE_FILES
    Musashi/m68k.h
    Musashi/m68kconf.h
    Musashi/m68kcpu.c
    Musashi/m68kcpu.h
    Musashi/m68kdasm.c
    Musashi/m68kopac.c
    Musashi/m68kopdm.c
    Musashi/m68kopnz.c
    Musashi/m68kops.c
    Musashi/m68kops.h
    vadm.cxx libs.cxx libs.h loader.cxx loader.h memory.cxx memory.h cpu.cxx cpu.h)

add_executable(vadm ${SOURCE_FILES})
target_link_libraries(vadm log4cxx PocoFoundation)