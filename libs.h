//
// VADE - classes representing the different system libraries
//
// Copyright(C) 2016 Constantin Wiemer
//


#include <iostream>
#include <stdint.h>
#include <log4cxx/logger.h>
#include <Poco/Format.h>
#include <Poco/File.h>

#include "memory.h"

extern "C"
{
#include "Musashi/m68k.h"
}


#ifndef VADE_LIBS_H
#define VADE_LIBS_H


#define ADDR_EXEC_BASE   0x00f00000
#define ADDR_DOS_BASE    0x00f10000

#define SWAP_BYTES(x) (((x) & 0x000000ff) << 24) | (((x) & 0x0000ff00) << 8) | (((x) & 0x00ff0000) >> 8) | (((x) & 0xff000000) >> 24)


// global logger
extern log4cxx::LoggerPtr g_logger;

// global pointer to memory
extern uint8_t *g_mem;

// global pointer to MemoryManager object
extern MemoryManager *g_memmgr;

// global map of opened libraries
class AmiLibrary;
extern std::map <uint32_t, AmiLibrary *> g_libmap;


std::string hexdump(const uint8_t *, size_t);


class AmiLibrary
{
public:
    void call(const uint16_t offset);

protected:
    typedef uint32_t (AmiLibrary::*FUNCPTR)();

    std::map <const uint16_t, FUNCPTR> m_funcmap;

};


class ExecLibrary : public AmiLibrary
{
public:
    ExecLibrary();

private:

    uint32_t OpenLibrary();
    uint32_t AllocVec();
    uint32_t FreeVec();
    uint32_t FindTask();
};


class DOSLibrary : public AmiLibrary
{
public:
    DOSLibrary()
    {
        m_funcmap[0x3b4] = (FUNCPTR) &DOSLibrary::PutStr;
        m_funcmap[0x054] = (FUNCPTR) &DOSLibrary::Lock;
        m_funcmap[0x05a] = (FUNCPTR) &DOSLibrary::UnLock;
    }

private:
    uint32_t PutStr();
    uint32_t IoErr();
    uint32_t Lock();
    uint32_t UnLock();
    uint32_t ParsePattern();
    uint32_t MatchPattern();
    uint32_t Examine();
    uint32_t ExNext();
};


#endif //VADE_LIBS_H
