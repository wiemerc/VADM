//
// VADE - Virtual Amiga DOS Environment
//
// This is just a proof-of-concept of an ABI emulator, so only a very small part of the actual Amiga DOS is emulated.
// It was inspired by Wine, the virtual DOS machine (VDM) in Windows NT and Wabi for Solaris. For the emulation
// of the CPU (Motorola M680x0) the code of the Musashi emulator is used.
//
// Copyright(C) 2016 Constantin Wiemer
//


// standard headers
#include <stdint.h>                     // can't use cstdint because it requires C++11 support
#include <iostream>
#include <vector>

// Apache log4cxx
#include "log4cxx/logger.h"
#include "log4cxx/propertyconfigurator.h"

// Poco libraries
#include "Poco/Format.h"
#include "Poco/FileStream.h"
#include "Poco/BinaryReader.h"

// project headers
#include "cpu.h"
#include "memory.h"
#include "reader.h"


// global logger
log4cxx::LoggerPtr g_logger;

// global pointer to memory
uint8_t *g_mem;

// global map of opened libraries
std::map <uint32_t, Library *> g_libmap;


//
// generate a hexdump from a buffer of bytes
//
std::string hexdump (const uint8_t *buffer, size_t length)
{
    std::string dump;
    size_t pos = 0;

    while (pos < length)
    {
        dump += Poco::format ("%04?x: ", pos);
        std::string line;
        for (size_t i = pos; (i < pos + 16) && (i < length); i++)
        {
            dump += Poco::format ("%02?x ", buffer[i]);
            if (buffer[i] >= 0x20 && buffer[i] <= 0x7e)
            {
                line.append (1, buffer[i]);
            }
            else
            {
                line.append (1, '.');
            }
        }
        if (line.size() < 16)
            dump.append (3 * (16 - line.size()), ' ');

        dump.append (1, '\t');
        dump += line;
        dump.append (1, '\n');
        pos += 16;
    }
    return dump;
}


int main(int argc, char *argv[])
{
    // setup logging
    g_logger = log4cxx::Logger::getLogger("tftpd");
    log4cxx::PropertyConfigurator::configure("logging.properties");

    // allocate memory for our VM and fill code area with STOP instructions
    g_mem = new uint8_t[ADDR_MEM_END - ADDR_MEM_START + 1];
    for (uint16_t *p = (uint16_t *) (g_mem + ADDR_CODE_START + 10); p < (uint16_t *) (g_mem + ADDR_CODE_END); ++p)
        *p = 0x724e;

    //
    // load executable
    //
    if (argc != 2) {
        LOG4CXX_ERROR(g_logger, "usage: vade <program>");
        return 1;
    }
    try
    {
        AmiHunkReader reader = AmiHunkReader();
        reader.read(argv[1], ADDR_CODE_START);
    }
    catch (std::exception &e)
    {
        LOG4CXX_FATAL(g_logger, "exception occurred while loading executable: " << e.what());
        return 1;
    }

    //
    // initialize CPU
    //
    m68k_init();
    m68k_set_cpu_type(M68K_CPU_TYPE_68000);
    // We need to initialize two special addresses where the CPU reads the initial values for its SSP and PC from upon reset.
    // On the Amiga this was done by shadowing these addresses to the ROM where the values were stored.
    // The initial SSP is the first address above the stack area because the stack grows from high to low addresses.
    m68k_write_32(ADDR_INITIAL_SSP, ADDR_STACK_END + 1);
    m68k_write_32(ADDR_INITIAL_PC, ADDR_CODE_START);
    m68k_pulse_reset();

    //
    // execute program
    //
    // TODO: We need to pass the CLI arguments to the program

    // open Exec library
    m68k_write_32(4, ADDR_EXEC_BASE);                                            // base of Exec library
    g_libmap[ADDR_EXEC_BASE] = new ExecLibrary();

    // setup jump table for library routines
    m68k_write_16(ADDR_EXEC_BASE - 0x228, 0x4e40);                               // TRAP + RTS for OpenLibrary()
    m68k_write_16(ADDR_EXEC_BASE - 0x226, 0x4e75);
    m68k_write_16(ADDR_DOS_BASE - 0x3b4, 0x4e40);                                // TRAP + RTS for PutStr()
    m68k_write_16(ADDR_DOS_BASE - 0x3b2, 0x4e75);
    m68k_write_16(ADDR_DOS_BASE - 0x090, 0x4e40);                                // TRAP + RTS for Exit()
    m68k_write_16(ADDR_DOS_BASE - 0x088, 0x4e75);
    m68k_write_32(ADDR_EXV_TRAP_0, ADDR_CODE_END - 1);                           // exception vector for traps
    m68k_write_16(ADDR_CODE_END - 1, 0x4e73);                                    // RTE instruction

    // setup stack
    m68k_set_reg(M68K_REG_SP, ADDR_STACK_END - 7);                               // decrement SP
    m68k_write_32(ADDR_STACK_END - 3, ADDR_STACK_END - ADDR_STACK_START + 1);    // stack size
    m68k_write_32(ADDR_STACK_END - 7, ADDR_CODE_END - 3);                        // return address (STOP instruction)

    // run program
    try
    {
        m68k_execute(INT32_MAX);
    }
    catch (std::exception &e)
    {
        LOG4CXX_FATAL(g_logger, "exception occurred while executing program: " << e.what());
        return 1;
    }

    return 0;
}
