//
// VADE - functions / classes for memory access and memory management
//
// Copyright(C) 2016 Constantin Wiemer
//


#include "memory.h"


extern "C"
{
    unsigned int m68k_read_8(unsigned int address)
    {
        LOG4CXX_TRACE(g_logger, Poco::format("8 bit read from address 0x%08x", address));
        if ((address >= ADDR_MEM_START) && (address <= ADDR_MEM_END))
            return READ_BYTE(g_mem, address);
        else
            return 0x55;
    }

    unsigned int m68k_read_16(unsigned int address)
    {
        LOG4CXX_TRACE(g_logger, Poco::format("16 bit read from address 0x%08x", address));
        if ((address >= ADDR_MEM_START) && (address <= ADDR_MEM_END - 1))
            return READ_WORD(g_mem, address);
        else
            return 0xdead;
    }

    unsigned int m68k_read_32(unsigned int address)
    {
        LOG4CXX_TRACE(g_logger, Poco::format("32 bit read from address 0x%08x", address));
        if ((address >= ADDR_MEM_START) && (address <= ADDR_MEM_END - 3))
            return READ_LONG(g_mem, address);
        else
            return 0xdeadbeef;
    }


    // memory access for the disassembler => no tracing
    unsigned int m68k_peek_16(unsigned int address)
    {
        if ((address >= ADDR_MEM_START) && (address <= ADDR_MEM_END - 1))
            return READ_WORD(g_mem, address);
        else
            return 0xdead;
    }

    // memory access for the disassembler => no tracing
    unsigned int m68k_peek_32(unsigned int address)
    {
        if ((address >= ADDR_MEM_START) && (address <= ADDR_MEM_END - 3))
            return READ_LONG(g_mem, address);
        else
            return 0xdeadbeef;
    }


    void m68k_write_8(unsigned int address, unsigned int value)
    {
        LOG4CXX_TRACE(g_logger, Poco::format("8 bit write to address 0x%08x, value = 0x%02x", address, value));
        if ((address >= ADDR_MEM_START) && (address <= ADDR_MEM_END))
            WRITE_BYTE(g_mem, address, value);
        else
        LOG4CXX_TRACE(g_logger, Poco::format("illegal write access to address 0x%08x", address));
    }

    void m68k_write_16(unsigned int address, unsigned int value)
    {
        LOG4CXX_TRACE(g_logger, Poco::format("16 bit write to address 0x%08x, value = 0x%04x", address, value));
        if ((address >= ADDR_MEM_START) && (address <= ADDR_MEM_END - 1)) {
            WRITE_WORD(g_mem, address, value);
        }
        else
        LOG4CXX_TRACE(g_logger, Poco::format("illegal write access to address 0x%08x", address));
    }

    void m68k_write_32(unsigned int address, unsigned int value)
    {
        LOG4CXX_TRACE(g_logger, Poco::format("32 bit write to address 0x%08x, value = 0x%08x", address, value));
        if ((address >= ADDR_MEM_START) && (address <= ADDR_MEM_END - 3)) {
            WRITE_LONG(g_mem, address, value);
        }
        else
        LOG4CXX_TRACE(g_logger, Poco::format("illegal write access to address 0x%08x", address));
    }
};


