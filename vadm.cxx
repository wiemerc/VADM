//
// VADM - Virtual Amiga DOS Machine
//
// VADM executes (simple) programs written and compiled for Amiga DOS / Amiga OS. This is just a proof-of-concept of an ABI emulator,
// so only a very small part of the actual Amiga DOS / Amiga OS is emulated. It was inspired by Wine, the virtual DOS machine (VDM)
// in Windows NT and Wabi for Solaris. For the emulation of the CPU (Motorola M680x0) the code of the Musashi emulator is used.
//
// Copyright(C) 2016 Constantin Wiemer
//


// standard headers
#include <cstdint>
#include <vector>
#include <stdexcept>

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
#include "loader.h"


// global logger
log4cxx::LoggerPtr g_logger;

// global pointer to memory
uint8_t *g_mem;

// global pointer to MemoryManager object
MemoryManager *g_memmgr;

// global map of opened libraries
std::map <uint32_t, AmiLibrary *> g_libmap;


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
    g_logger = log4cxx::Logger::getLogger("vadm");
    log4cxx::PropertyConfigurator::configure("logging.properties");

    // create memory manager
    g_memmgr = new MemoryManager();

    //
    // load executable
    //
    if (argc < 2) {
        LOG4CXX_ERROR(g_logger, "usage: vadm <program> [arguments]");
        return 1;
    }
    LOG4CXX_INFO(g_logger, "loading executable...");
    try
    {
        AmiHunkLoader loader;
        loader.load(argv[1], ADDR_CODE_START);
    }
    catch (std::exception &e)
    {
        LOG4CXX_FATAL(g_logger, "exception occurred while loading executable: " << e.what());
        return 1;
    }

    //
    // initialize CPU
    //
    LOG4CXX_INFO(g_logger, "initializing CPU...");
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
    LOG4CXX_INFO(g_logger, "executing program...");

    // pass the command line to the program
    // On the Amiga the command line was passed as one string in A0 (the pointer)
    // and the length in D0. So we need to concatenate all but the first elements
    // of argv and store them in one string in the VM's memory, separated by spaces.
    // TODO: Use the Poco application class and it's argument vector
    char *cmdline = (char *) g_memmgr->alloc(1024);
    char *bufptr = cmdline;
    uint32_t bufsize = 1023;                    // one byte less than the buffer size to leave room for the terminating NUL byte
    ++argv;                                     // skip program name
    while (*argv != NULL) {
        uint32_t arglen = strlen(*argv);
        if (arglen + 1 < bufsize) {             // arglen + 1 to account for the additional space
            strncpy(bufptr, *argv, bufsize);    // copy argument
            bufptr  += arglen;
            bufsize -= arglen;
            strncpy(bufptr, " ", bufsize);      // add space
            ++bufptr;
            --bufsize;
            ++argv;
        }
        else {
            LOG4CXX_FATAL(g_logger, "command line too long");
            return 1;
        }
    }
    cmdline[1023] = 0;                          // terminate string in all cases
    LOG4CXX_DEBUG(g_logger, "command line: " << cmdline);
    m68k_set_reg(M68K_REG_A0, PTR_HOST_TO_M68K(cmdline));
    m68k_set_reg(M68K_REG_D0, strlen(cmdline));
    
    // open Exec library
    m68k_write_32(4, ADDR_EXEC_BASE);                                            // base of Exec library
    g_libmap[ADDR_EXEC_BASE] = new ExecLibrary(ADDR_EXEC_BASE);

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
