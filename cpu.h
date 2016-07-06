//
// VADE - functions related to the CPU
//
// Copyright(C) 2016 Constantin Wiemer
//


#include <stdint.h>
#include <string>
#include <log4cxx/logger.h>
#include <Poco/Format.h>
#include "libs.h"


#ifndef VADE_CPU_H
#define VADE_CPU_H


// global logger
extern log4cxx::LoggerPtr g_logger;

// global map of opened libraries
extern std::map <uint32_t, AmiLibrary *> g_libmap;


extern "C"
{
    void m68k_instr_callback();
    void m68k_trap_callback();
}


#endif //VADE_CPU_H
