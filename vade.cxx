//
// VADE - Virtual Amiga DOS Environment
//
// This is just a proof-of-concept of an ABI emulator, so only a very small part of the actual Amiga DOS is emulated.
// It was inspired by the virtual DOS machine (VDM) in Windows NT. For the emulation of the CPU (Motorola M680x0) the
// Musashi emulator is used.
//
// Copyright(C) 2016 Constantin Wiemer
//


// standard headers
#include <stdint.h>
#include <iostream>
#include <vector>

// Apache log4cxx
#include "log4cxx/logger.h"
#include "log4cxx/propertyconfigurator.h"

// Poco libraries
#include "Poco/Format.h"
#include "Poco/FileStream.h"
#include "Poco/BinaryReader.h"

#include "libs.h"
#include "reader.h"

extern "C"
{
// Musashi headers
#include "Musashi/m68k.h"

// Amiga headers
#include "dos/doshunks.h"
};


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


// global logger
log4cxx::LoggerPtr g_logger;

// global pointer to memory
uint8_t *g_mem;

// global map of opened libraries
std::map <uint32_t, Library *> g_libmap;


// macros for reading / writing data
#define READ_BYTE(BASE, ADDR) (BASE)[ADDR]
#define READ_WORD(BASE, ADDR) (((BASE)[ADDR]<<8) | (BASE)[(ADDR)+1])
#define READ_LONG(BASE, ADDR) (((BASE)[ADDR]<<24) | ((BASE)[(ADDR)+1]<<16) | ((BASE)[(ADDR)+2]<<8) | (BASE)[(ADDR)+3])

#define WRITE_BYTE(BASE, ADDR, VAL) (BASE)[ADDR] = (VAL)&0xff
#define WRITE_WORD(BASE, ADDR, VAL) (BASE)[ADDR] = ((VAL)>>8) & 0xff; (BASE)[(ADDR)+1] = (VAL)&0xff
#define WRITE_LONG(BASE, ADDR, VAL) (BASE)[ADDR] = ((VAL)>>24) & 0xff; (BASE)[(ADDR)+1] = ((VAL)>>16)&0xff; (BASE)[(ADDR)+2] = ((VAL)>>8)&0xff;	 (BASE)[(ADDR)+3] = (VAL)&0xff



extern "C"
{
void m68k_instr_callback()
{
    char instr[100];
    std::string dump;
    unsigned int ipc, pc;
    unsigned int nbytes;

    ipc = pc = m68k_get_reg(NULL, M68K_REG_PC);
    nbytes = m68k_disassemble(instr, ipc, M68K_CPU_TYPE_68000);
    while (nbytes > 0) {
        dump += Poco::format("%04x ", m68k_read_disassembler_16(pc));
        nbytes -= 2;
        pc += 2;
    }
    LOG4CXX_DEBUG(g_logger, Poco::format("next instruction at 0x%06x: %-20s: %s", ipc, dump, std::string(instr)));
}


void m68k_libcall_callback(unsigned int vector)
{
    unsigned int base = m68k_get_reg(NULL, M68K_REG_A6);
    unsigned int offset = m68k_get_reg(NULL, M68K_REG_A6) - m68k_get_reg(NULL, M68K_REG_PC) + 2;
    LOG4CXX_DEBUG(g_logger, Poco::format("trap occurred, base address = 0x%08x, offset = 0x%04x", base, offset));
    if (g_libmap.find(base) != g_libmap.end())
        g_libmap[base]->call(offset);
    else {
        LOG4CXX_ERROR(g_logger, Poco::format("base address 0x%08x not found in map of opened libraries", base));
        throw std::runtime_error("bad library call");
    }
}


// TODO: throw exception in case of illegal memory access
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


// needed for the m68k_disassemble() function
unsigned int m68k_read_disassembler_16(unsigned int address)
{
    if ((address >= ADDR_MEM_START) && (address <= ADDR_MEM_END - 1))
        return READ_WORD(g_mem, address);
    else
        return 0xdead;
}

unsigned int m68k_read_disassembler_32(unsigned int address)
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
