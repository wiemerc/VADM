cmake_minimum_required(VERSION 3.5)
project(VADE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

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
    vade.c)

add_executable(vade ${SOURCE_FILES})