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
}


//
// OpenLibrary
// A1: library name
// D0: version (not used)
// returns: pointer to library or 0
//
uint32_t ExecLibrary::OpenLibrary()
{
    LOG4CXX_DEBUG(g_logger, "ExecLibrary::OpenLibrary() has been called");
    const char *libname    = (const char *) PTR_M68K_TO_HOST(m68k_get_reg(NULL, M68K_REG_A1));
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
    LOG4CXX_DEBUG(g_logger, "ExecLibrary::AllocVec() has been called");
    const uint32_t size  = m68k_get_reg(NULL, M68K_REG_D0);
    const uint32_t flags = m68k_get_reg(NULL, M68K_REG_D1);
    LOG4CXX_DEBUG(g_logger, "size = " << size << ", flags = " << Poco::format("0x%08x", flags));

    try {
        uint8_t * ptr = g_memmgr->alloc(size);
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
    LOG4CXX_DEBUG(g_logger, "ExecLibrary::FreeVec() has been called");
    const uint32_t ptr = m68k_get_reg(NULL, M68K_REG_A1);
    LOG4CXX_DEBUG(g_logger, Poco::format("ptr = 0x%08x", ptr));

    g_memmgr->free(PTR_M68K_TO_HOST(ptr));
    return 0;
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
    LOG4CXX_DEBUG(g_logger, "DOSLibrary::PutStr() has been called");
    const char *str = (const char *) PTR_M68K_TO_HOST(m68k_get_reg(NULL, M68K_REG_D1));
    std::cout << str;
    return 0;
}


//
// Lock
// D1: path to file or directory
// D2: access mode (not used)
// returns: BPTR to struct FileLock or 0 in case of an error
//
uint32_t DOSLibrary::Lock()
{
    LOG4CXX_DEBUG(g_logger, "DOSLibrary::Lock() has been called");
    const char *path    = (const char *) PTR_M68K_TO_HOST(m68k_get_reg(NULL, M68K_REG_D1));
    const uint32_t mode = SWAP_BYTES(m68k_get_reg(NULL, M68K_REG_D2));
    LOG4CXX_DEBUG(g_logger, "path = " << path << ", mode = " << mode);

    // As Lock() was typically used to Examine() a file or directory, and this does not require a lock on neither
    // Unix nor Windows, we don't really lock anything here but only create a Poco::File object (which can be used
    // in Examine() to create a Poco::DirectoryIterator) and store the pointer in the fl_Key field of the FileLock
    // structure to associate the lock with the file or directory. We set fl_Task to NULL because no DirectoryIterator
    // object has been created yet.
    Poco::File *obj = new Poco::File(path);
    if (obj->exists()) {
        LOG4CXX_DEBUG(g_logger, "creating lock for file / dir '" << path << "'");
        struct FileLock *lock = ((struct FileLock *) g_memmgr->alloc(sizeof(struct FileLock)));
        lock->fl_Key    = (uint32_t) obj;
        lock->fl_Access = SWAP_BYTES(mode);
        lock->fl_Task   = NULL;
        return PTR_C_TO_BCPL(PTR_HOST_TO_M68K(lock));

    }
    else {
        LOG4CXX_ERROR(g_logger, "could not create lock for file / dir '" << path << "' because it does not exist");
        return 0;
    }
}


uint32_t DOSLibrary::UnLock()
{
    LOG4CXX_DEBUG(g_logger, "DOSLibrary::UnLock() has been called");
    const struct FileLock *lock = (struct FileLock *) PTR_M68K_TO_HOST(PTR_BCPL_TO_C(m68k_get_reg(NULL, M68K_REG_D1)));

    Poco::File *obj = (Poco::File *) lock->fl_Key;
    LOG4CXX_DEBUG(g_logger, "unlocking file / dir '" << obj->path() << "'");
    delete obj;
    if (lock->fl_Task) {
        Poco::DirectoryIterator *it = (Poco::DirectoryIterator *) lock->fl_Task;
        delete it;
    }
    g_memmgr->free((uint8_t *) lock);
    return 0;
}


//
// Examine
// D1: BPTR to struct FileLock
// D2: pointer to struct FileInfoBlock
// D2: access mode (not used)
// returns: > 0 or 0 in case of an error
//
uint32_t DOSLibrary::Examine()
{
    LOG4CXX_DEBUG(g_logger, "DOSLibrary::Examine() has been called");
    struct FileLock *lock = (struct FileLock *) PTR_M68K_TO_HOST(PTR_BCPL_TO_C(m68k_get_reg(NULL, M68K_REG_D1)));
    struct FileInfoBlock *fib   = (struct FileInfoBlock *) PTR_M68K_TO_HOST(m68k_get_reg(NULL, M68K_REG_D2));

    Poco::File *obj = (Poco::File *) lock->fl_Key;

    // fill FileInfoBlock with information of the current object
    // TODO: What was the size of directory in AmigaDOS?
    // TODO: Add flags and timestamp
    strncpy(fib->fib_FileName, Poco::Path(obj->path()).getFileName().c_str(), 100);
    if (obj->isFile())
        fib->fib_Size = SWAP_BYTES(obj->getSize());         // This is only correct if the file is smaller than 4GB
    else
        fib->fib_Size = 0;
    if (obj->isDirectory()) {
        // TODO: Use correct values for dirs and files
        fib->fib_DirEntryType = +1;
        // create DirectoryIterator object and store the pointer in fl_Task field of the lock. This of course breaks
        // programs which use this field to find out the handler that owns the lock...
        Poco::DirectoryIterator *it = new Poco::DirectoryIterator(*obj);
        lock->fl_Task = (struct MsgPort *) it;
    }
    else
        fib->fib_DirEntryType = -1;                         // We treat special files as regular files

    return 1;
}


uint32_t DOSLibrary::ExNext()
{
    LOG4CXX_DEBUG(g_logger, "DOSLibrary::ExNext() has been called");
    struct FileLock *lock = (struct FileLock *) PTR_M68K_TO_HOST(PTR_BCPL_TO_C(m68k_get_reg(NULL, M68K_REG_D1)));
    struct FileInfoBlock *fib = (struct FileInfoBlock *) PTR_M68K_TO_HOST(m68k_get_reg(NULL, M68K_REG_D2));

    Poco::DirectoryIterator *it = (Poco::DirectoryIterator *) lock->fl_Task;

    // check if there are more entries
    if (*it == Poco::DirectoryIterator()) {
        m_errno = ERROR_NO_MORE_ENTRIES;
        return 0;
    }

    // fill FileInfoBlock with information of the current object
    LOG4CXX_DEBUG(g_logger, "current object: " << it->path().getFileName());
    strncpy(fib->fib_FileName, it->path().getFileName().c_str(), 100);
    if ((*it)->isFile())
        fib->fib_Size = SWAP_BYTES((*it)->getSize());       // This is only correct if the file is smaller than 4GB
    else
        fib->fib_Size = 0;
    if ((*it)->isDirectory())
        fib->fib_DirEntryType = +1;
    else
        fib->fib_DirEntryType = -1;                         // We treat special files as regular files

    // move iterator to next entry
    ++(*it);
    return 1;
}


uint32_t DOSLibrary::IoErr()
{
    LOG4CXX_DEBUG(g_logger, "DOSLibrary::IoErr() has been called");
    return m_errno;
}


uint32_t DOSLibrary::ParsePattern()
{
    return 0;
}


uint32_t DOSLibrary::MatchPattern()
{
    return 0;
}

