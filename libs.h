//
// VADE - classes representing the different system libraries
//
// Copyright(C) 2016 Constantin Wiemer
//


#include <iostream>
#include <stdint.h>
#include <log4cxx/logger.h>
#include <Poco/Format.h>

extern "C"
{
#include "Musashi/m68k.h"
}


#ifndef VADE_LIBS_H
#define VADE_LIBS_H


#define ADDR_EXEC_BASE   0x00f00000
#define ADDR_DOS_BASE    0x00f10000


class Library
{
public:
    void call(const uint16_t offset);

protected:
    typedef uint32_t (Library::*FUNCPTR)();

    std::map <const uint16_t, FUNCPTR> m_funcmap;

};


class ExecLibrary : public Library
{
public:
    ExecLibrary()
    {
        m_funcmap[0x228] = (FUNCPTR) &ExecLibrary::OpenLibrary;
    }

private:
    uint32_t OpenLibrary();
};


class DOSLibrary : public Library
{
public:
    DOSLibrary()
    {
        m_funcmap[0x3b4] = (FUNCPTR) &DOSLibrary::PutStr;
    }

private:
    uint32_t PutStr();
};


#endif //VADE_LIBS_H
