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

// Apache log4cxx
#include "log4cxx/logger.h"
#include "log4cxx/propertyconfigurator.h"

// Poco libraries
#include "Poco/Format.h"
#include "Poco/FileStream.h"
#include "Poco/BinaryReader.h"

extern "C"
{
// Musashi headers
#include "Musashi/m68k.h"

// Amiga headers
#include "dos/doshunks.h"
};


// memory layout
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


// global logger
log4cxx::LoggerPtr g_logger;

// global pointer to memory
static uint8_t *g_mem;


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


// TODO: throw exception in case of illegal memory access
unsigned int m68k_read_8(unsigned int address)
{
    LOG4CXX_DEBUG(g_logger, Poco::format("8 bit read from address 0x%08x", address));
    if ((address >= ADDR_MEM_START) && (address <= ADDR_MEM_END))
        return READ_BYTE(g_mem, address);
    else
        return 0x55;
}

unsigned int m68k_read_16(unsigned int address)
{
    LOG4CXX_DEBUG(g_logger, Poco::format("16 bit read from address 0x%08x", address));
    if ((address >= ADDR_MEM_START) && (address <= ADDR_MEM_END - 1))
        return READ_WORD(g_mem, address);
    else
        return 0xdead;
}

unsigned int m68k_read_32(unsigned int address)
{
    LOG4CXX_DEBUG(g_logger, Poco::format("32 bit read from address 0x%08x", address));
    // We need to detect two special addresses where the CPU reads the initial values for its SSP and PC from upon reset.
    // On the Amiga this was done by shadowing these addresses to the ROM where the values were stored.
    if (address == ADDR_INITIAL_SSP)
        // The initial SSP is the first address above the stack area because the stack grows from high to low addresses.
        return ADDR_STACK_END + 1;
    else if (address == ADDR_INITIAL_PC)
        return ADDR_CODE_START;
    else if ((address >= ADDR_MEM_START) && (address <= ADDR_MEM_END - 3))
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
    LOG4CXX_DEBUG(g_logger, Poco::format("8 bit write to address 0x%08x, value = 0x%02x", address, value));
    if ((address >= ADDR_MEM_START) && (address <= ADDR_MEM_END))
        WRITE_BYTE(g_mem, address, value);
    else
        LOG4CXX_DEBUG(g_logger, Poco::format("illegal write access to address 0x%08x", address));
}

void m68k_write_16(unsigned int address, unsigned int value)
{
    LOG4CXX_DEBUG(g_logger, Poco::format("16 bit write to address 0x%08x, value = 0x%04x", address, value));
    if ((address >= ADDR_MEM_START) && (address <= ADDR_MEM_END - 1)) {
        WRITE_WORD(g_mem, address, value);
    }
    else
        LOG4CXX_DEBUG(g_logger, Poco::format("illegal write access to address 0x%08x", address));
}

void m68k_write_32(unsigned int address, unsigned int value)
{
    LOG4CXX_DEBUG(g_logger, Poco::format("32 bit write to address 0x%08x, value = 0x%08x", address, value));
    if ((address >= ADDR_MEM_START) && (address <= ADDR_MEM_END - 3)) {
        WRITE_LONG(g_mem, address, value);
    }
    else
        LOG4CXX_DEBUG(g_logger, Poco::format("illegal write access to address 0x%08x", address));
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

    // allocate memory for our VM
    g_mem = new uint8_t[ADDR_MEM_END - ADDR_MEM_START + 1];

    //
    // load executable
    //
    if (argc != 2) {
        LOG4CXX_ERROR(g_logger, "usage: vade <program>");
        return 1;
    }
    try
    {
        Poco::FileInputStream exe(argv[1]);
        Poco::BinaryReader::BinaryReader reader(exe, Poco::BinaryReader::BIG_ENDIAN_BYTE_ORDER);
        uint32_t htype;
        while (!reader.eof()) {
            reader >> htype;
            switch (htype)
            {
                case HUNK_HEADER:
                    LOG4CXX_INFO(g_logger, "hunk type = HUNK_HEADER");
                    uint32_t lword;
                    reader >> lword;
                    LOG4CXX_DEBUG(g_logger, "long words reserved for resident libraries: " << lword);
                    reader >> lword;
                    LOG4CXX_DEBUG(g_logger, "number of hunks: " << lword);
                    uint32_t fhunk;
                    reader >> fhunk;
                    LOG4CXX_DEBUG(g_logger, "number of first hunk: " << fhunk);
                    uint32_t lhunk;
                    reader >> lhunk;
                    LOG4CXX_DEBUG(g_logger, "number of last hunk: " << lhunk);
                    for (int i = fhunk; i <= lhunk; i++) {
                        reader >> lword;
                        LOG4CXX_DEBUG(g_logger, "size (in bytes) of hunk #" << i << ": " << lword * 4);
                    }
                    break;

                case HUNK_CODE:
                    LOG4CXX_INFO(g_logger, "hunk type = HUNK_CODE");
                    uint32_t nwords;
                    reader >> nwords;
                    LOG4CXX_DEBUG(g_logger, "size (in bytes) of code hunk: " << nwords * 4);
                    reader.readRaw((char *) g_mem + ADDR_CODE_START, nwords * 4);
                    LOG4CXX_TRACE(g_logger, "hex dump of code:\n" << hexdump(g_mem + ADDR_CODE_START, nwords * 4));
                    break;

                case HUNK_DATA:
                    LOG4CXX_INFO(g_logger, "hunk type = HUNK_DATA");
                    LOG4CXX_ERROR(g_logger, "hunk type is not implemented");
                    break;

                case HUNK_BSS:
                    LOG4CXX_INFO(g_logger, "hunk type = HUNK_BSS");
                    LOG4CXX_ERROR(g_logger, "hunk type is not implemented");
                    break;

                case HUNK_RELOC32:
                    LOG4CXX_INFO(g_logger, "hunk type = HUNK_RELOC32");
                    LOG4CXX_ERROR(g_logger, "hunk type is not implemented");
                    break;

                case HUNK_END:
                    LOG4CXX_INFO(g_logger, "hunk type = HUNK_END");
                    break;

                default:
                    LOG4CXX_ERROR(g_logger, "unknown hunk type: " << htype);
            }
        }
    }
    catch (std::exception &e)
    {
        LOG4CXX_FATAL(g_logger, "exception occurred while loading executable: " << e.what());
        return 1;
    }

/*
    // test code (in reversed byte oder)
    uint16_t *p = (uint16_t *) (g_mem + ADDR_CODE_START);
    *p++ = 0x7c2c;      // move.l #$deadbeef, a6
    *p++ = 0xadde;
    *p++ = 0xefbe;
    *p++ = 0x564e;      // link a6, #$0
    *p++ = 0x0000;
    // fill rest of code area with NOPs
    for (p = (uint16_t *) (g_mem + ADDR_CODE_START + 10); p < (uint16_t *) (g_mem + ADDR_CODE_END); ++p)
        *p = 0x714e;
*/

    m68k_init();
    m68k_set_cpu_type(M68K_CPU_TYPE_68000);
    m68k_pulse_reset();
    m68k_execute(1000);
    LOG4CXX_TRACE(g_logger, "stack after execution of the program:\n" << hexdump(g_mem + ADDR_STACK_END - 32, 32));

    return 0;
}
