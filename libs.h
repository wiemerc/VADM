//
// VADE - classes representing the different system libraries
//
// Copyright(C) 2016 Constantin Wiemer
//


#include <iostream>
#include <stdint.h>
#include <log4cxx/logger.h>
#include <Poco/Format.h>
#include <Poco/Path.h>
#include <Poco/File.h>

extern "C"
{
#include "Musashi/m68k.h"
}


#ifndef VADE_LIBS_H
#define VADE_LIBS_H


#define ADDR_EXEC_BASE   0x00f00000
#define ADDR_DOS_BASE    0x00f10000
#define ADDR_HEAP_START  0x00000400     // lowest 1k is reserved for the CPU (exception and interrupt vectors)



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
    uint8_t * AllocVecInt(const uint32_t size);
    void FreeVecInt(uint8_t *block);

private:
    typedef struct
    {
        bool     mcb_is_free;
        uint32_t mcb_size;
    } MEMORY_CONTROLL_BLOCK;

    uint8_t *m_last_mem_addr;

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
