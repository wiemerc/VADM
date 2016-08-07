//
// VADM - class for loading the executable in Amiga Hunk format
//
// Copyright(C) 2016 Constantin Wiemer
//


#include <stdint.h>
#include <log4cxx/logger.h>
#include <Poco/Format.h>
#include <Poco/FileStream.h>
#include <Poco/BinaryReader.h>

extern "C"
{
#include <dos/doshunks.h>
}


#ifndef VADM_LOADER_H
#define VADM_LOADER_H


std::string hexdump(const uint8_t *, size_t);
extern "C"
{
unsigned int m68k_read_32(unsigned int address);
void m68k_write_32(unsigned int address, unsigned int value);
}


class AmiHunkLoader
{
public:
    void load(char *fname, uint32_t loc);
};


#endif //VADM_LOADER_H
