//
// VADE - classes representing the different system libraries
//
// Copyright(C) 2016 Constantin Wiemer
//


#include "libs.h"
#include "memory.h"

// Amiga OS headers
// We need to define _SYS_TIME_H_ to avoid overriding the definition of struct timeval by <devices/timer.h>
// (which is included by <dos/dosextens.h>)
extern "C"
{
#define _SYS_TIME_H_
#include <exec/memory.h>
#include <dos/dosextens.h>
}


// global logger
extern log4cxx::LoggerPtr g_logger;

// global pointer to memory
extern uint8_t *g_mem;

// global map of opened libraries
extern std::map <uint32_t, AmiLibrary *> g_libmap;


//
// methods of AmiLibrary
//
// TODO: Libraries should have a list of offsets to fill the map and setup the jump table
void AmiLibrary::call(const uint16_t offset)
{
    uint32_t rc;
    if (m_funcmap.find(offset) != m_funcmap.end()) {
        rc = (this->*m_funcmap[offset])();
        m68k_set_reg(M68K_REG_D0, rc);
    }
    else {
        LOG4CXX_ERROR(g_logger, Poco::format("library routine with offset 0x%04x not found in map", (unsigned int) offset));
        throw std::runtime_error("bad library call");
    }
}


//
// methods of ExecLibrary
//

ExecLibrary::ExecLibrary()
{
    // add functions to map
    m_funcmap[0x228] = (FUNCPTR) &ExecLibrary::OpenLibrary;
    m_funcmap[0x2ac] = (FUNCPTR) &ExecLibrary::AllocVec;
    m_funcmap[0x2b2] = (FUNCPTR) &ExecLibrary::FreeVec;

    // initialize memory pool
    m_last_mem_addr = PTR_M68K_TO_HOST(ADDR_HEAP_START);
}


//
// OpenLibrary
// A1: library name
// D0: version (not used)
// returns: pointer to library or 0
//
uint32_t ExecLibrary::OpenLibrary()
{
    LOG4CXX_DEBUG(g_logger, "ExecLibrary::OpenLibrary() was called");
    const char *libname = (const char *) PTR_M68K_TO_HOST(m68k_get_reg(NULL, M68K_REG_A1));
    const uint32_t version = m68k_get_reg(NULL, M68K_REG_D0);
    LOG4CXX_DEBUG(g_logger, "library name = " << libname << ", version = " << version);

    if (strcmp(libname, "dos.library") == 0) {
        LOG4CXX_DEBUG(g_logger, "opening dos.library");
        g_libmap[ADDR_DOS_BASE] = new DOSLibrary();
        return ADDR_DOS_BASE;
    }
    else {
        // TODO: Should we throw an exception in case of errors?
        LOG4CXX_ERROR(g_logger, "unknown library: " << libname);
        return 0;
    }
}


uint32_t ExecLibrary::AllocVec()
{
    LOG4CXX_DEBUG(g_logger, "ExecLibrary::AllocVec() was called");
    const uint32_t size = m68k_get_reg(NULL, M68K_REG_D0);
    const uint32_t flags = m68k_get_reg(NULL, M68K_REG_D1);
    LOG4CXX_DEBUG(g_logger, "size = " << size << ", flags = " << Poco::format("0x%08x", flags));

    try {
        uint8_t * ptr = AllocVecInt(size);
        // We ignore all flags except MEMF_CLEAR
        if (flags & MEMF_CLEAR)
            memset(ptr, 0, size);
        return PTR_HOST_TO_M68K(ptr);
    }
    catch (std::exception &e) {
        return 0;
    }

}


uint32_t ExecLibrary::FreeVec()
{
    LOG4CXX_DEBUG(g_logger, "ExecLibrary::FreeVec() was called");
    const uint32_t ptr = m68k_get_reg(NULL, M68K_REG_A1);
    LOG4CXX_DEBUG(g_logger, Poco::format("ptr = 0x%08x", ptr));

    FreeVecInt(PTR_M68K_TO_HOST(ptr));
    return 0;
}


uint32_t ExecLibrary::FindTask()
{
    return 0;
}


uint8_t *ExecLibrary::AllocVecInt(const uint32_t size)
{
    // This very simple algorithm for memory allocation is based on this article: http://www.ibm.com/developerworks/library/l-memory/
    // It is not suitable for a real application (because of fragmentation and probably also performance).
    uint8_t *ptr = PTR_M68K_TO_HOST(ADDR_HEAP_START);
    MEMORY_CONTROLL_BLOCK *mcb;

    // walk through list of previously allocated blocks and see if there is a free one that fits
    while (ptr != m_last_mem_addr) {
        mcb = (MEMORY_CONTROLL_BLOCK *) ptr;
        if (mcb->mcb_is_free && (mcb->mcb_size >= size)) {
            mcb->mcb_is_free = false;
//            LOG4CXX_DEBUG(g_logger, Poco::format("reusing block of %d bytes at address 0x%08x from pool", mcb->mcb_size, PTR_HOST_TO_M68K(ptr)));
            LOG4CXX_DEBUG(g_logger, "reusing block of " << mcb->mcb_size << " bytes at address " << PTR_HOST_TO_M68K(ptr) << " from pool");
            return ptr + sizeof(MEMORY_CONTROLL_BLOCK);
        }
        ptr += sizeof(MEMORY_CONTROLL_BLOCK) + mcb->mcb_size;
    }

    // no suitable block found => allocate a new one
    if ((ADDR_HEAP_END - PTR_HOST_TO_M68K(m_last_mem_addr)) >= (sizeof(MEMORY_CONTROLL_BLOCK) + size)) {
        mcb = (MEMORY_CONTROLL_BLOCK *) ptr;        // already points to m_last_mem_addr
        mcb->mcb_is_free = false;
        mcb->mcb_size    = size;
        m_last_mem_addr += sizeof(MEMORY_CONTROLL_BLOCK) + size;
//        LOG4CXX_DEBUG(g_logger, Poco::format("allocating block of %d bytes at address 0x%08x from pool", mcb->mcb_size, PTR_HOST_TO_M68K(ptr)));
        LOG4CXX_DEBUG(g_logger, "allocating block of " << mcb->mcb_size << " bytes at address " << PTR_HOST_TO_M68K(ptr) << " from pool");
        return ptr + sizeof(MEMORY_CONTROLL_BLOCK);
    }
    else {
        LOG4CXX_FATAL(g_logger, "out of memory - could not allocate block of " << size << " bytes");
        throw std::runtime_error("out of memory");
    }
}


void ExecLibrary::FreeVecInt(uint8_t *block)
{
    MEMORY_CONTROLL_BLOCK *mcb = (MEMORY_CONTROLL_BLOCK *) (block - sizeof(MEMORY_CONTROLL_BLOCK));
    mcb->mcb_is_free = true;
}


//
// methods of DOSLibrary
//

//
// PutStr
// D1: string
// returns: 0 or -1 in case of an error
//
uint32_t DOSLibrary::PutStr()
{
    LOG4CXX_DEBUG(g_logger, "DOSLibrary::PutStr() was called");
    const char *str = (const char *) PTR_M68K_TO_HOST(m68k_get_reg(NULL, M68K_REG_D1));
    std::cout << str;
    return 0;
}


uint32_t DOSLibrary::IoErr()
{
    return 0;
}


uint32_t DOSLibrary::Lock()
{
    LOG4CXX_DEBUG(g_logger, "DOSLibrary::Lock() was called");
    const char *path = (const char *) PTR_M68K_TO_HOST(m68k_get_reg(NULL, M68K_REG_D1));
    const uint32_t mode = m68k_get_reg(NULL, M68K_REG_D2);
    LOG4CXX_DEBUG(g_logger, "path = " << path << ", mode = " << mode);

    struct FileLock *lock = ((struct FileLock *) ((ExecLibrary *) g_libmap[ADDR_EXEC_BASE])->AllocVecInt( sizeof(struct FileLock)));
    lock->fl_Key = 4711;

    return PTR_HOST_TO_M68K(lock);
}


uint32_t DOSLibrary::UnLock()
{
    LOG4CXX_DEBUG(g_logger, "DOSLibrary::UnLock() was called");
    return 0;
}


uint32_t DOSLibrary::ParsePattern()
{
    return 0;
}


uint32_t DOSLibrary::MatchPattern()
{
    return 0;
}


uint32_t DOSLibrary::Examine()
{
    return 0;
}


uint32_t DOSLibrary::ExNext()
{
    return 0;
}

