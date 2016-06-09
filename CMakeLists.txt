cmake_minimum_required(VERSION 3.5)
project(VADE)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -I/usr/local/include -I/opt/m68k-amigaos/os-include")

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
    vade.cxx libs.cxx libs.h reader.cxx reader.h memory.cxx memory.h cpu.cxx cpu.h)

add_executable(vade ${SOURCE_FILES})
target_link_libraries(vade log4cxx PocoFoundation)