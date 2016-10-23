//
// VADE - functions / classes for memory access and memory management
//
// Copyright(C) 2016 Constantin Wiemer
//


#include "memory.h"


//
// methods of MemoryManager
//
MemoryManager::MemoryManager()
{
    // initialize memory pool
    MemoryManager::m_lastMemAddr = PTR_M68K_TO_HOST(ADDR_HEAP_START);
}


uint8_t *MemoryManager::alloc(uint32_t size)
{
    // This simple algorithm for memory allocation is based on this article: http://www.ibm.com/developerworks/library/l-memory/
    // It is not suitable for a real application (because of fragmentation and probably also performance).
    // We always allocate at least MEMORY_MIN_BLOCK_SIZE bytes.
    if (size < MEMORY_MIN_BLOCK_SIZE)
        size = MEMORY_MIN_BLOCK_SIZE;

    uint8_t *ptr = PTR_M68K_TO_HOST(ADDR_HEAP_START);
    MEMORY_CONTROLL_BLOCK *mcb, *newmcb;

    // walk through list of previously allocated blocks and see if there is a free one that fits
    while (ptr != m_lastMemAddr) {
        mcb = (MEMORY_CONTROLL_BLOCK *) ptr;
        if (mcb->mcb_isFree && (mcb->mcb_size >= size)) {
            mcb->mcb_isFree = false;
            // If this block can hold both the requested amount of bytes and at least MEMORY_MIN_BLOCK_SIZE bytes we split it.
            if ((mcb->mcb_size - size) >= MEMORY_MIN_BLOCK_SIZE) {
                LOG4CXX_DEBUG(g_logger, Poco::format("splitting block of %u bytes at address 0x%08x", mcb->mcb_size, PTR_HOST_TO_M68K(ptr)));
                newmcb = (MEMORY_CONTROLL_BLOCK *) (ptr + sizeof(MEMORY_CONTROLL_BLOCK) + size);
                newmcb->mcb_isFree = true;
                newmcb->mcb_size   = mcb->mcb_size - size - sizeof(MEMORY_CONTROLL_BLOCK);
                mcb->mcb_size = size;
            }
            LOG4CXX_DEBUG(g_logger, Poco::format("reusing block of %u bytes at address 0x%08x from pool", mcb->mcb_size, PTR_HOST_TO_M68K(ptr)));
            return ptr + sizeof(MEMORY_CONTROLL_BLOCK);
        }
        ptr += sizeof(MEMORY_CONTROLL_BLOCK) + mcb->mcb_size;
    }

    // no suitable block found => allocate a new one
    if ((ADDR_HEAP_END - PTR_HOST_TO_M68K(m_lastMemAddr)) >= (sizeof(MEMORY_CONTROLL_BLOCK) + size)) {
        mcb = (MEMORY_CONTROLL_BLOCK *) ptr;        // already points to m_lastMemAddr
        mcb->mcb_isFree = false;
        mcb->mcb_size    = size;
        m_lastMemAddr += sizeof(MEMORY_CONTROLL_BLOCK) + size;
        LOG4CXX_DEBUG(g_logger, Poco::format("allocating block of %u bytes at address 0x%08x from pool", mcb->mcb_size, PTR_HOST_TO_M68K(ptr)));
        return ptr + sizeof(MEMORY_CONTROLL_BLOCK);
    }
    else {
        LOG4CXX_FATAL(g_logger, "out of memory - could not allocate block of " << size << " bytes");
        throw std::runtime_error("out of memory");
    }
}

void MemoryManager::free(uint8_t *ptr)
{
    MEMORY_CONTROLL_BLOCK *mcb = (MEMORY_CONTROLL_BLOCK *) (ptr - sizeof(MEMORY_CONTROLL_BLOCK));
    mcb->mcb_isFree = true;
}


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


