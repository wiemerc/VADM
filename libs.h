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
#include <Poco/DirectoryIterator.h>
#include <Poco/DateTime.h>
#include <Poco/DateTimeParser.h>

#include "memory.h"

extern "C"
{
#include "Musashi/m68k.h"

// Amiga OS headers
// We need to define _SYS_TIME_H_ to avoid overriding the definition of struct timeval by <devices/timer.h>
// (which is included by <dos/dosextens.h>)
#define _SYS_TIME_H_
#include <exec/memory.h>
#include <dos/dosextens.h>
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
    ExecLibrary(uint32_t base);

private:

    uint32_t OpenLibrary();
    uint32_t CloseLibrary();
    uint32_t AllocVec();
    uint32_t FreeVec();
    uint32_t FindTask();
    uint32_t Forbid();
    uint32_t Permit();
    uint32_t ReplyMsg();
};


class DOSLibrary : public AmiLibrary
{
public:
    DOSLibrary(uint32_t base);

private:
    uint32_t m_errno;

    void getFileInfo(const Poco::File &obj, struct FileInfoBlock *);

    uint32_t PutStr();
    uint32_t IoErr();
    uint32_t Lock();
    uint32_t UnLock();
    uint32_t Examine();
    uint32_t ExNext();
};


#endif //VADE_LIBS_H
