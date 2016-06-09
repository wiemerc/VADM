//
// VADE - functions / classes for memory access and memory management
//
// Copyright(C) 2016 Constantin Wiemer
//


#include <stdint.h>
#include <string>
#include <log4cxx/logger.h>
#include <Poco/Format.h>
#include "libs.h"
extern "C"
{
#include "Musashi/m68k.h"
}


#ifndef VADE_MEMORY_H
#define VADE_MEMORY_H


// memory layout
// TODO: Do we need to have separate areas for code and heap?
#define ADDR_MEM_START   0x00000000
#define ADDR_MEM_END     0x00ffffff     // 16MB memory (because the M68000 only had 24 address bits)
#define ADDR_HEAP_START  0x00000000
#define ADDR_HEAP_END    0x003fffff     // 4MB heap
#define ADDR_STACK_START 0x00400000
#define ADDR_STACK_END   0x007fffff     // 4MB stack (grows from high to low addresses)
#define ADDR_CODE_START  0x00800000
#define ADDR_CODE_END    0x00ffffff     // 8MB code
#define ADDR_INITIAL_SSP 0x00000000     // address that contains the initial value for the SSP upon reset of the CPU
#define ADDR_INITIAL_PC  0x00000004     // address that contains the initial value for the PC upon reset of the CPU
#define ADDR_EXV_TRAP_0   0x00000080    // exception vector for trap #0 (used for the library calls)


// macros for reading / writing data
#define READ_BYTE(BASE, ADDR) (BASE)[ADDR]
#define READ_WORD(BASE, ADDR) (((BASE)[ADDR]<<8) | (BASE)[(ADDR)+1])
#define READ_LONG(BASE, ADDR) (((BASE)[ADDR]<<24) | ((BASE)[(ADDR)+1]<<16) | ((BASE)[(ADDR)+2]<<8) | (BASE)[(ADDR)+3])

#define WRITE_BYTE(BASE, ADDR, VAL) (BASE)[ADDR] = (VAL)&0xff
#define WRITE_WORD(BASE, ADDR, VAL) (BASE)[ADDR] = ((VAL)>>8) & 0xff; (BASE)[(ADDR)+1] = (VAL)&0xff
#define WRITE_LONG(BASE, ADDR, VAL) (BASE)[ADDR] = ((VAL)>>24) & 0xff; (BASE)[(ADDR)+1] = ((VAL)>>16)&0xff; (BASE)[(ADDR)+2] = ((VAL)>>8)&0xff;	 (BASE)[(ADDR)+3] = (VAL)&0xff


// global logger
extern log4cxx::LoggerPtr g_logger;

// global pointer to memory
extern uint8_t *g_mem;

// global map of opened libraries
extern std::map <uint32_t, Library *> g_libmap;


extern "C"
{
    unsigned int m68k_read_8(unsigned int address);
    unsigned int m68k_read_16(unsigned int address);
    unsigned int m68k_read_32(unsigned int address);
    unsigned int m68k_peek_16(unsigned int address);
    unsigned int m68k_peek_32(unsigned int address);
    void m68k_write_8(unsigned int address, unsigned int value);
    void m68k_write_16(unsigned int address, unsigned int value);
    void m68k_write_32(unsigned int address, unsigned int value);
}


#endif //VADE_MEMORY_H
