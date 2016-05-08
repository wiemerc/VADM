//
// VADE - Virtual Amiga DOS Environment
//
// This is just a proof-of-concept of an ABI emulator, so only a very small part of the actual Amiga DOS is emulated.
// It was inspired by the virtual DOS machine (VDM) in Windows NT. For the emulation of the CPU (Motorola M680x0) the
// Musashi emulator is used.
//
// Copyright(C) 2016 Constantin Wiemer
//


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "Musashi/m68k.h"


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


// global pointer to memory
static uint8_t *g_mem;


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
    printf("next instruction at 0x%06x: ", pc);
    instr_size = m68k_disassemble(instr, pc, M68K_CPU_TYPE_68000);
    // TODO: hexdump shows only last word of instruction
    for (p = hexdump; instr_size > 0; instr_size -= 2, p +=4, pc += 2) {
        sprintf(hexdump, "%04x", m68k_read_disassembler_16(pc)); 
        if (instr_size > 2)
            *p++ = ' ';
    }
    printf("%-20s: %s\n", hexdump, instr);
    fflush(stdout);
}


unsigned int m68k_read_memory_8(unsigned int address)
{
    printf("8 bit read from address 0x%08x - NOT IMPLEMENTED\n", address);
    return 0x55;
}

unsigned int m68k_read_memory_16(unsigned int address)
{
    printf("16 bit read from address 0x%08x\n", address);
    if ((address >= ADDR_MEM_START) && (address <= ADDR_MEM_END - 1))
        return READ_WORD(g_mem, address);
    else
        return 0xdead;
}

unsigned int m68k_read_memory_32(unsigned int address)
{
    printf("32 bit read from address 0x%08x\n", address);
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


void m68k_write_memory_8(unsigned int address, unsigned int value)
{
    printf("8 bit write to address 0x%08x, value = 0x%02x\n", address, value);
    if ((address >= ADDR_MEM_START) && (address <= ADDR_MEM_END))
        WRITE_BYTE(g_mem, address, value);
    else
        printf("illegal write access to address 0x%08x", address);
}

void m68k_write_memory_16(unsigned int address, unsigned int value)
{
    printf("16 bit write to address 0x%08x, value = 0x%04x\n", address, value);
    if ((address >= ADDR_MEM_START) && (address <= ADDR_MEM_END - 1)) {
        WRITE_WORD(g_mem, address, value);
    }
    else
        printf("illegal write access to address 0x%08x", address);
}

void m68k_write_memory_32(unsigned int address, unsigned int value)
{
    printf("32 bit write to address 0x%08x, value = 0x%08x\n", address, value);
    if ((address >= ADDR_MEM_START) && (address <= ADDR_MEM_END - 3)) {
        WRITE_LONG(g_mem, address, value);
    }
    else
        printf("illegal write access to address 0x%08x", address);
}


int main(int argc, char *argv[])
{
    uint16_t *p;

    // allocate memory
    if (!(g_mem = malloc(ADDR_MEM_END - ADDR_MEM_START + 1))) {
        printf("could not allocate memory\n");
        return 1;
    }

    // test code (in reversed byte oder)
    p = (uint16_t *) (g_mem + ADDR_CODE_START);
    *p++ = 0x7c2c;      // move.l #$deadbeef, a6
    *p++ = 0xadde;
    *p++ = 0xefbe;
    *p++ = 0x564e;      // link a6, #$0
    *p++ = 0x0000;
    // fill rest of code area with NOPs
    for (p = (uint16_t *) (g_mem + ADDR_CODE_START + 10); p < (uint16_t *) (g_mem + ADDR_CODE_END); ++p)
        *p = 0x714e;

    m68k_init();
    m68k_set_cpu_type(M68K_CPU_TYPE_68000);
    m68k_pulse_reset();
    m68k_execute(100);

    return 0;
}
