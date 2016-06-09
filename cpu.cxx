//
// VADE - functions / classes related to the CPU
//
// Copyright(C) 2016 Constantin Wiemer
//


#include "cpu.h"


void m68k_instr_callback()
{
    char instr[100];
    std::string dump;
    unsigned int ipc, pc;
    unsigned int nbytes;

    ipc = pc = m68k_get_reg(NULL, M68K_REG_PC);
    nbytes = m68k_disassemble(instr, ipc, M68K_CPU_TYPE_68000);
    while (nbytes > 0) {
        dump += Poco::format("%04x ", m68k_peek_16(pc));
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

