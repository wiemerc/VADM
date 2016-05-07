//
// VADE - Virtual Amiga DOS Environment
//
// This is just a proof-of-concept of a ABI emulator, so only a very small part of the actual Amiga DOS is emulated.
// It was inspired by the virtual DOS machine (VDM) in Windows NT. For the emulation of the CPU (Motorola M680x0) the
// Musashi emulator is used.
//
// Copyright(C) 2016 Constantin Wiemer
//


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

#include "Musashi/m68k.h"


// addresses according to the Amiga memory layout
#define ADDR_RAM_START   0x00000000
#define ADDR_RAM_END     0x003fffff
#define ADDR_ROM_START   0x00bfffff
#define ADDR_ROM_END     0x00ffffff
#define ADDR_STACK_START 0x00100000     // TODO: What was used on the Amiga?
#define ADDR_INITIAL_SSP 0x00000000     // address that contains the initial value for the SSP upon reset of the CPU
#define ADDR_INITIAL_PC  0x00000004     // address that contains the initial value for the PC upon reset of the CPU


// global pointers to ROM and RAM
static uint8_t *g_rom;
static uint8_t *g_ram;


// macros for reading / writing data
#define READ_BYTE(BASE, ADDR) (BASE)[ADDR]
#define READ_WORD(BASE, ADDR) (((BASE)[ADDR]<<8) | (BASE)[(ADDR)+1])
#define READ_LONG(BASE, ADDR) (((BASE)[ADDR]<<24) | ((BASE)[(ADDR)+1]<<16) | ((BASE)[(ADDR)+2]<<8) | (BASE)[(ADDR)+3])

#define WRITE_BYTE(BASE, ADDR, VAL) (BASE)[ADDR] = (VAL)&0xff
#define WRITE_WORD(BASE, ADDR, VAL) (BASE)[ADDR] = ((VAL)>>8) & 0xff; (BASE)[(ADDR)+1] = (VAL)&0xff
#define WRITE_LONG(BASE, ADDR, VAL) (BASE)[ADDR] = ((VAL)>>24) & 0xff; (BASE)[(ADDR)+1] = ((VAL)>>16)&0xff; (BASE)[(ADDR)+2] = ((VAL)>>8)&0xff;	 (BASE)[(ADDR)+3] = (VAL)&0xff


void m68k_instr_callback()
{
    static char instr[100];
    static char hexdump[100];
    static char *p;
    static unsigned int pc;
    static unsigned int instr_size;

    pc = m68k_get_reg(NULL, M68K_REG_PC);
    instr_size = m68k_disassemble(instr, pc, M68K_CPU_TYPE_68000);
    for (p = hexdump; instr_size > 0; instr_size -= 2, p +=4, pc += 2) {
        sprintf(hexdump, "%04x", m68k_read_disassembler_16(pc));
        if (instr_size > 2)
            *p++ = ' ';
    }
    printf("next instruction at 0x%06x: %-20s: %s\n", pc, hexdump, instr);
    fflush(stdout);
}


unsigned int m68k_read_memory_8(unsigned int address)
{
    printf("8 bit read from address 0x%08x\n", address);
    return 0x55;
}

unsigned int m68k_read_memory_16(unsigned int address)
{
    printf("16 bit read from address 0x%08x\n", address);
    if ((address >= ADDR_ROM_START) && (address <= ADDR_ROM_END - 1))
        return READ_WORD(g_rom, address - ADDR_ROM_START);
    else
        return 0xdead;
}

unsigned int m68k_read_memory_32(unsigned int address)
{
    printf("32 bit read from address 0x%08x\n", address);
    // We need to detect two special addresses where the CPU reads the initial values for its SSP and PC from upon reset.
    // On the Amiga this was done by shadowing these addresses to the ROM where the values were stored.
    if (address == ADDR_INITIAL_SSP)
        return ADDR_STACK_START;
    else if (address == ADDR_INITIAL_PC)
        return ADDR_ROM_START;
    else if ((address >= ADDR_ROM_START) && (address <= ADDR_ROM_END - 3))
        return READ_LONG(g_rom, address - ADDR_ROM_START);
    else
        return 0xdeadbeef;
}


// needed for the m68k_disassemble() function
unsigned int m68k_read_disassembler_16(unsigned int address) {m68k_read_memory_16(address);}
unsigned int m68k_read_disassembler_32(unsigned int address) {m68k_read_memory_32(address);}


void m68k_write_memory_8(unsigned int address, unsigned int value)
{
    printf("8 bit write to address 0x%08x, value = 0x%02x\n", address, value);
    if ((address >= ADDR_RAM_START) && (address <= ADDR_RAM_END))
        WRITE_BYTE(g_ram, address, value);
    else
        printf("illegal write access to address 0x%08x", address);
}

void m68k_write_memory_16(unsigned int address, unsigned int value)
{
    printf("16 bit write to address 0x%08x, value = 0x%04x\n", address, value);
    if ((address >= ADDR_RAM_START) && (address <= ADDR_RAM_END - 1)) {
        WRITE_WORD(g_ram, address, value);
    }
    else
        printf("illegal write access to address 0x%08x", address);
}

void m68k_write_memory_32(unsigned int address, unsigned int value)
{
    printf("32 bit write to address 0x%08x, value = 0x%08x\n", address, value);
    if ((address >= ADDR_RAM_START) && (address <= ADDR_RAM_END - 3)) {
        WRITE_LONG(g_ram, address, value);
    }
    else
        printf("illegal write access to address 0x%08x", address);
}


int main(int argc, char *argv[])
{
    uint16_t *p;

    if (!(g_rom = malloc(ADDR_ROM_END - ADDR_ROM_START + 1))) {
        printf("could not allocate code memory\n");
        return 1;
    }
    for (p = (uint16_t *)g_rom; p < (uint16_t *)(g_rom + ADDR_ROM_END - ADDR_ROM_START); ++p)
        *p = 0x714e;

    if (!(g_ram = malloc(ADDR_RAM_END - ADDR_RAM_START + 1))) {
        printf("could not allocate data memory\n");
        return 1;
    }

    m68k_init();
    m68k_set_cpu_type(M68K_CPU_TYPE_68000);
    m68k_pulse_reset();
    m68k_execute(100);

    return 0;
}
