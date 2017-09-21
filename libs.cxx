//
// VADE - classes representing the different system libraries
//
// Copyright(C) 2016 Constantin Wiemer
//


#include "libs.h"

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
void AmiLibrary::call(const uint16_t offset)
{
    uint32_t rc;
    if (m_funcmap.find(offset) != m_funcmap.end()) {
		if (m_funcmap[offset] == nullptr) {
	        LOG4CXX_ERROR(g_logger, Poco::format("library routine with offset 0x%x not implemented", (unsigned int) offset));
            throw std::runtime_error("bad library call");
		}
        rc = (this->*m_funcmap[offset])();
        m68k_set_reg(M68K_REG_D0, rc);
    }
    else {
        LOG4CXX_ERROR(g_logger, Poco::format("library routine with offset 0x%08x not found in map", (unsigned int) offset));
        throw std::runtime_error("bad library call");
    }
}


//
// methods of ExecLibrary
//

ExecLibrary::ExecLibrary(uint32_t base)
{
    // add functions to map
    m_funcmap[0x228] = (FUNCPTR) &ExecLibrary::OpenLibrary;
	m_funcmap[0x19e] = (FUNCPTR) &ExecLibrary::CloseLibrary;
    m_funcmap[0x2ac] = (FUNCPTR) &ExecLibrary::AllocVec;
    m_funcmap[0x2b2] = (FUNCPTR) &ExecLibrary::FreeVec;
    m_funcmap[0x126] = (FUNCPTR) &ExecLibrary::FindTask;
	m_funcmap[0x84]  = (FUNCPTR) &ExecLibrary::Forbid;
	m_funcmap[0x8a]  = (FUNCPTR) &ExecLibrary::Permit;
	m_funcmap[0x17a] = (FUNCPTR) &ExecLibrary::ReplyMsg;
	m_funcmap[0xea] = (FUNCPTR) &ExecLibrary::Insert;
	m_funcmap[0xfc] = (FUNCPTR) &ExecLibrary::Remove;
	m_funcmap[0xf0] = (FUNCPTR) &ExecLibrary::AddHead;
	m_funcmap[0x102] = (FUNCPTR) &ExecLibrary::RemHead;

    // lines below have been generated with the following command line:
    // grep syscall exec_pragmas.h | perl -nale 'print "m_funcmap[0x$F[3]] = nullptr;    // $F[2]"'
	m_funcmap[0x1e] = nullptr;    // Supervisor
	m_funcmap[0x48] = nullptr;    // InitCode
	m_funcmap[0x4e] = nullptr;    // InitStruct
	m_funcmap[0x54] = nullptr;    // MakeLibrary
	m_funcmap[0x5a] = nullptr;    // MakeFunctions
	m_funcmap[0x60] = nullptr;    // FindResident
	m_funcmap[0x66] = nullptr;    // InitResident
	m_funcmap[0x6c] = nullptr;    // Alert
	m_funcmap[0x72] = nullptr;    // Debug
	m_funcmap[0x78] = nullptr;    // Disable
	m_funcmap[0x7e] = nullptr;    // Enable
	m_funcmap[0x90] = nullptr;    // SetSR
	m_funcmap[0x96] = nullptr;    // SuperState
	m_funcmap[0x9c] = nullptr;    // UserState
	m_funcmap[0xa2] = nullptr;    // SetIntVector
	m_funcmap[0xa8] = nullptr;    // AddIntServer
	m_funcmap[0xae] = nullptr;    // RemIntServer
	m_funcmap[0xb4] = nullptr;    // Cause
	m_funcmap[0xba] = nullptr;    // Allocate
	m_funcmap[0xc0] = nullptr;    // Deallocate
	m_funcmap[0xc6] = nullptr;    // AllocMem
	m_funcmap[0xcc] = nullptr;    // AllocAbs
	m_funcmap[0xd2] = nullptr;    // FreeMem
	m_funcmap[0xd8] = nullptr;    // AvailMem
	m_funcmap[0xde] = nullptr;    // AllocEntry
	m_funcmap[0xe4] = nullptr;    // FreeEntry
	m_funcmap[0xf6] = nullptr;    // AddTail
	m_funcmap[0x108] = nullptr;    // RemTail
	m_funcmap[0x10e] = nullptr;    // Enqueue
	m_funcmap[0x114] = nullptr;    // FindName
	m_funcmap[0x11a] = nullptr;    // AddTask
	m_funcmap[0x120] = nullptr;    // RemTask
	m_funcmap[0x12c] = nullptr;    // SetTaskPri
	m_funcmap[0x132] = nullptr;    // SetSignal
	m_funcmap[0x138] = nullptr;    // SetExcept
	m_funcmap[0x13e] = nullptr;    // Wait
	m_funcmap[0x144] = nullptr;    // Signal
	m_funcmap[0x14a] = nullptr;    // AllocSignal
	m_funcmap[0x150] = nullptr;    // FreeSignal
	m_funcmap[0x156] = nullptr;    // AllocTrap
	m_funcmap[0x15c] = nullptr;    // FreeTrap
	m_funcmap[0x162] = nullptr;    // AddPort
	m_funcmap[0x168] = nullptr;    // RemPort
	m_funcmap[0x16e] = nullptr;    // PutMsg
	m_funcmap[0x174] = nullptr;    // GetMsg
	m_funcmap[0x180] = nullptr;    // WaitPort
	m_funcmap[0x186] = nullptr;    // FindPort
	m_funcmap[0x18c] = nullptr;    // AddLibrary
	m_funcmap[0x192] = nullptr;    // RemLibrary
	m_funcmap[0x198] = nullptr;    // OldOpenLibrary
	m_funcmap[0x1a4] = nullptr;    // SetFunction
	m_funcmap[0x1aa] = nullptr;    // SumLibrary
	m_funcmap[0x1b0] = nullptr;    // AddDevice
	m_funcmap[0x1b6] = nullptr;    // RemDevice
	m_funcmap[0x1bc] = nullptr;    // OpenDevice
	m_funcmap[0x1c2] = nullptr;    // CloseDevice
	m_funcmap[0x1c8] = nullptr;    // DoIO
	m_funcmap[0x1ce] = nullptr;    // SendIO
	m_funcmap[0x1d4] = nullptr;    // CheckIO
	m_funcmap[0x1da] = nullptr;    // WaitIO
	m_funcmap[0x1e0] = nullptr;    // AbortIO
	m_funcmap[0x1e6] = nullptr;    // AddResource
	m_funcmap[0x1ec] = nullptr;    // RemResource
	m_funcmap[0x1f2] = nullptr;    // OpenResource
	m_funcmap[0x20a] = nullptr;    // RawDoFmt
	m_funcmap[0x210] = nullptr;    // GetCC
	m_funcmap[0x216] = nullptr;    // TypeOfMem
	m_funcmap[0x21c] = nullptr;    // Procure
	m_funcmap[0x222] = nullptr;    // Vacate
	m_funcmap[0x22e] = nullptr;    // InitSemaphore
	m_funcmap[0x234] = nullptr;    // ObtainSemaphore
	m_funcmap[0x23a] = nullptr;    // ReleaseSemaphore
	m_funcmap[0x240] = nullptr;    // AttemptSemaphore
	m_funcmap[0x246] = nullptr;    // ObtainSemaphoreList
	m_funcmap[0x24c] = nullptr;    // ReleaseSemaphoreList
	m_funcmap[0x252] = nullptr;    // FindSemaphore
	m_funcmap[0x258] = nullptr;    // AddSemaphore
	m_funcmap[0x25e] = nullptr;    // RemSemaphore
	m_funcmap[0x264] = nullptr;    // SumKickData
	m_funcmap[0x26a] = nullptr;    // AddMemList
	m_funcmap[0x270] = nullptr;    // CopyMem
	m_funcmap[0x276] = nullptr;    // CopyMemQuick
	m_funcmap[0x27c] = nullptr;    // CacheClearU
	m_funcmap[0x282] = nullptr;    // CacheClearE
	m_funcmap[0x288] = nullptr;    // CacheControl
	m_funcmap[0x28e] = nullptr;    // CreateIORequest
	m_funcmap[0x294] = nullptr;    // DeleteIORequest
	m_funcmap[0x29a] = nullptr;    // CreateMsgPort
	m_funcmap[0x2a0] = nullptr;    // DeleteMsgPort
	m_funcmap[0x2a6] = nullptr;    // ObtainSemaphoreShared

    // setup jump table (TRAP and RTS instructions for each routine)
    for (auto it = m_funcmap.begin(); it != m_funcmap.end(); ++it) {
        m68k_write_16(base - it->first, 0x4e40);
        m68k_write_16(base - it->first + 2, 0x4e75);
    }
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
        g_libmap[ADDR_DOS_BASE] = new DOSLibrary(ADDR_DOS_BASE);
        return ADDR_DOS_BASE;
    }
    else {
        LOG4CXX_ERROR(g_logger, "library not implemented: " << libname);
        throw std::runtime_error("library not implemented");
    }
}


uint32_t ExecLibrary::CloseLibrary()
{
    LOG4CXX_DEBUG(g_logger, "ExecLibrary::CloseLibrary() has been called");
    uint32_t lib = m68k_get_reg(NULL, M68K_REG_A1);
    if(g_libmap.find(lib) != g_libmap.end()) {
        g_libmap.erase(lib);
    }
    else {
        LOG4CXX_WARN(g_logger, "library with base address " << Poco::format("0x%08x", lib) << " has not been opened");
    }
    return 0;
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


uint32_t ExecLibrary::FindTask()
{
    LOG4CXX_DEBUG(g_logger, "ExecLibrary::FindTask() has been called");
    // We don't convert the pointer to a host pointer yet so that we can check for a NULL pointer.
    const char *taskname = (const char *) m68k_get_reg(NULL, M68K_REG_A1);
    if (taskname == nullptr) {
        // create a process structure with pr_CLI not NULL to indicate the program has been
        // started from the CLI and return a pointer to it
        LOG4CXX_DEBUG(g_logger, "task name is NULL - creating process structure");
        struct Process *proc = ((struct Process *) g_memmgr->alloc(sizeof(struct Process)));
        // The startup code of libnix means to check if pr_CLI is not NULL in order to
        // distinguish between a start from the Workbench and a start from the CLI. But
        // it uses the offset 172 which corresponds to pr_COS according to the docs for
        // OS 3.5. Hence we set pr_COS below.
        proc->pr_COS = 0xffffffff;
        return PTR_HOST_TO_M68K(proc);
    }
    else {
        // finding another process by name (there aren't any anyway...) is not supported
        LOG4CXX_WARN(g_logger, "task name is '" << PTR_M68K_TO_HOST(taskname) << "' - returning NULL");
        return 0;
    }
}


uint32_t ExecLibrary::Forbid()
{
    LOG4CXX_DEBUG(g_logger, "ExecLibrary::Forbid() has been called");
    return 0;
}


uint32_t ExecLibrary::Permit()
{
    LOG4CXX_DEBUG(g_logger, "ExecLibrary::Permit() has been called");
    return 0;
}


uint32_t ExecLibrary::ReplyMsg()
{
    LOG4CXX_DEBUG(g_logger, "ExecLibrary::ReplyMsg() has been called");
    // TODO: For now this routine has no functionality because it's only called by the
    //       startup code to reply the intial message to the Workbench. Once we
    //       implement messaging, we need to implement the correct functionality.
    return 0;
}


//
// list management functions of ExecLibrary (algorithms taken from Wikipedia)
//

uint32_t ExecLibrary::AddHead()
{
    LOG4CXX_DEBUG(g_logger, "ExecLibrary::AddHead() has been called");
    struct List *list = (struct List *) PTR_M68K_TO_HOST(m68k_get_reg(NULL, M68K_REG_A0));
    struct Node *node = (struct Node *) PTR_M68K_TO_HOST(m68k_get_reg(NULL, M68K_REG_A1));
    struct Node *before = (struct Node *) PTR_M68K_TO_HOST(list->lh_Head);
    list->lh_Head = (struct Node *) PTR_HOST_TO_M68K(node);
    if (list->lh_Head == nullptr) {
        list->lh_Tail = (struct Node *) PTR_HOST_TO_M68K(node);
        node->ln_Pred = nullptr;
        node->ln_Succ = nullptr;
    }
    else {
        node->ln_Pred = before->ln_Pred;
        node->ln_Succ = (struct Node *) PTR_HOST_TO_M68K(before);
        before->ln_Pred = (struct Node *) PTR_HOST_TO_M68K(node);
    }
    return 0;
}


uint32_t ExecLibrary::RemHead()
{
    LOG4CXX_DEBUG(g_logger, "ExecLibrary::RemHead() has been called");
    struct List *list = (struct List *) PTR_M68K_TO_HOST(m68k_get_reg(NULL, M68K_REG_A0));
    struct Node *node = (struct Node *) PTR_M68K_TO_HOST(list->lh_Head);
    list->lh_Head = node->ln_Succ;
    if (list->lh_Head != nullptr) {
        ((struct Node *) PTR_M68K_TO_HOST(list->lh_Head))->ln_Pred = nullptr;
    }
    return PTR_HOST_TO_M68K(node);
}


uint32_t ExecLibrary::Insert()
{
    LOG4CXX_DEBUG(g_logger, "ExecLibrary::Insert() has been called");
    struct List *list = (struct List *) PTR_M68K_TO_HOST(m68k_get_reg(NULL, M68K_REG_A0));
    struct Node *node = (struct Node *) PTR_M68K_TO_HOST(m68k_get_reg(NULL, M68K_REG_A1));
    struct Node *after = (struct Node *) m68k_get_reg(NULL, M68K_REG_A2);
    if (after == nullptr) {
        // special case: insert node at the head of the list
        AddHead();
    }
    else {
        after = (struct Node *) PTR_M68K_TO_HOST(after);
        node->ln_Pred = (struct Node *) PTR_HOST_TO_M68K(after);
        node->ln_Succ = after->ln_Succ;
        if (after->ln_Succ == nullptr) {
            list->lh_Tail = (struct Node *) PTR_HOST_TO_M68K(node);
        }
        else {
            ((struct Node *) PTR_M68K_TO_HOST(after->ln_Succ))->ln_Pred = (struct Node *) PTR_HOST_TO_M68K(node);
        }
        after->ln_Succ = (struct Node *) PTR_HOST_TO_M68K(node);
    }
    return 0;
}


uint32_t ExecLibrary::Remove()
{
    LOG4CXX_DEBUG(g_logger, "ExecLibrary::Remove() has been called");
    struct Node *node = (struct Node *) PTR_M68K_TO_HOST(m68k_get_reg(NULL, M68K_REG_A1));
    // TODO: What about the list if the node is the head / tail node?
    if (node->ln_Pred != nullptr) {
        ((struct Node *) PTR_M68K_TO_HOST(node->ln_Pred))->ln_Succ = node->ln_Succ;
    }
    if (node->ln_Succ != nullptr) {
        ((struct Node *) PTR_M68K_TO_HOST(node->ln_Succ))->ln_Pred = node->ln_Pred;
    }
    return 0;
}


//
// methods of DOSLibrary
//

DOSLibrary::DOSLibrary(uint32_t base) {
    m_funcmap[0x3b4] = (FUNCPTR) &DOSLibrary::PutStr;
    m_funcmap[0x054] = (FUNCPTR) &DOSLibrary::Lock;
    m_funcmap[0x05a] = (FUNCPTR) &DOSLibrary::UnLock;
    m_funcmap[0x066] = (FUNCPTR) &DOSLibrary::Examine;
    m_funcmap[0x06c] = (FUNCPTR) &DOSLibrary::ExNext;
    m_funcmap[0x084] = (FUNCPTR) &DOSLibrary::IoErr;

    // lines below have been generated with the following command line:
    // grep libcall dos_pragmas.h | perl -nale 'print "m_funcmap[0x$F[4]] = nullptr;    // $F[3]"'
    m_funcmap[0x1e] = nullptr;    // Open
    m_funcmap[0x24] = nullptr;    // Close
    m_funcmap[0x2a] = nullptr;    // Read
    m_funcmap[0x30] = nullptr;    // Write
    m_funcmap[0x36] = nullptr;    // Input
    m_funcmap[0x3c] = nullptr;    // Output
    m_funcmap[0x42] = nullptr;    // Seek
    m_funcmap[0x48] = nullptr;    // DeleteFile
    m_funcmap[0x4e] = nullptr;    // Rename
    m_funcmap[0x60] = nullptr;    // DupLock
    m_funcmap[0x72] = nullptr;    // Info
    m_funcmap[0x78] = nullptr;    // CreateDir
    m_funcmap[0x7e] = nullptr;    // CurrentDir
    m_funcmap[0x8a] = nullptr;    // CreateProc
    m_funcmap[0x90] = nullptr;    // Exit
    m_funcmap[0x96] = nullptr;    // LoadSeg
    m_funcmap[0x9c] = nullptr;    // UnLoadSeg
    m_funcmap[0xae] = nullptr;    // DeviceProc
    m_funcmap[0xb4] = nullptr;    // SetComment
    m_funcmap[0xba] = nullptr;    // SetProtection
    m_funcmap[0xc0] = nullptr;    // DateStamp
    m_funcmap[0xc6] = nullptr;    // Delay
    m_funcmap[0xcc] = nullptr;    // WaitForChar
    m_funcmap[0xd2] = nullptr;    // ParentDir
    m_funcmap[0xd8] = nullptr;    // IsInteractive
    m_funcmap[0xde] = nullptr;    // Execute
    m_funcmap[0xe4] = nullptr;    // AllocDosObject
    m_funcmap[0xe4] = nullptr;    // AllocDosObjectTagList
    m_funcmap[0xea] = nullptr;    // FreeDosObject
    m_funcmap[0xf0] = nullptr;    // DoPkt
    m_funcmap[0xf0] = nullptr;    // DoPkt0
    m_funcmap[0xf0] = nullptr;    // DoPkt1
    m_funcmap[0xf0] = nullptr;    // DoPkt2
    m_funcmap[0xf0] = nullptr;    // DoPkt3
    m_funcmap[0xf0] = nullptr;    // DoPkt4
    m_funcmap[0xf6] = nullptr;    // SendPkt
    m_funcmap[0xfc] = nullptr;    // WaitPkt
    m_funcmap[0x102] = nullptr;    // ReplyPkt
    m_funcmap[0x108] = nullptr;    // AbortPkt
    m_funcmap[0x10e] = nullptr;    // LockRecord
    m_funcmap[0x114] = nullptr;    // LockRecords
    m_funcmap[0x11a] = nullptr;    // UnLockRecord
    m_funcmap[0x120] = nullptr;    // UnLockRecords
    m_funcmap[0x126] = nullptr;    // SelectInput
    m_funcmap[0x12c] = nullptr;    // SelectOutput
    m_funcmap[0x132] = nullptr;    // FGetC
    m_funcmap[0x138] = nullptr;    // FPutC
    m_funcmap[0x13e] = nullptr;    // UnGetC
    m_funcmap[0x144] = nullptr;    // FRead
    m_funcmap[0x14a] = nullptr;    // FWrite
    m_funcmap[0x150] = nullptr;    // FGets
    m_funcmap[0x156] = nullptr;    // FPuts
    m_funcmap[0x15c] = nullptr;    // VFWritef
    m_funcmap[0x162] = nullptr;    // VFPrintf
    m_funcmap[0x168] = nullptr;    // Flush
    m_funcmap[0x16e] = nullptr;    // SetVBuf
    m_funcmap[0x174] = nullptr;    // DupLockFromFH
    m_funcmap[0x17a] = nullptr;    // OpenFromLock
    m_funcmap[0x180] = nullptr;    // ParentOfFH
    m_funcmap[0x186] = nullptr;    // ExamineFH
    m_funcmap[0x18c] = nullptr;    // SetFileDate
    m_funcmap[0x192] = nullptr;    // NameFromLock
    m_funcmap[0x198] = nullptr;    // NameFromFH
    m_funcmap[0x19e] = nullptr;    // SplitName
    m_funcmap[0x1a4] = nullptr;    // SameLock
    m_funcmap[0x1aa] = nullptr;    // SetMode
    m_funcmap[0x1b0] = nullptr;    // ExAll
    m_funcmap[0x1b6] = nullptr;    // ReadLink
    m_funcmap[0x1bc] = nullptr;    // MakeLink
    m_funcmap[0x1c2] = nullptr;    // ChangeMode
    m_funcmap[0x1c8] = nullptr;    // SetFileSize
    m_funcmap[0x1ce] = nullptr;    // SetIoErr
    m_funcmap[0x1d4] = nullptr;    // Fault
    m_funcmap[0x1da] = nullptr;    // PrintFault
    m_funcmap[0x1e0] = nullptr;    // ErrorReport
    m_funcmap[0x1ec] = nullptr;    // Cli
    m_funcmap[0x1f2] = nullptr;    // CreateNewProc
    m_funcmap[0x1f2] = nullptr;    // CreateNewProcTagList
    m_funcmap[0x1f8] = nullptr;    // RunCommand
    m_funcmap[0x1fe] = nullptr;    // GetConsoleTask
    m_funcmap[0x204] = nullptr;    // SetConsoleTask
    m_funcmap[0x20a] = nullptr;    // GetFileSysTask
    m_funcmap[0x210] = nullptr;    // SetFileSysTask
    m_funcmap[0x216] = nullptr;    // GetArgStr
    m_funcmap[0x21c] = nullptr;    // SetArgStr
    m_funcmap[0x222] = nullptr;    // FindCliProc
    m_funcmap[0x228] = nullptr;    // MaxCli
    m_funcmap[0x22e] = nullptr;    // SetCurrentDirName
    m_funcmap[0x234] = nullptr;    // GetCurrentDirName
    m_funcmap[0x23a] = nullptr;    // SetProgramName
    m_funcmap[0x240] = nullptr;    // GetProgramName
    m_funcmap[0x246] = nullptr;    // SetPrompt
    m_funcmap[0x24c] = nullptr;    // GetPrompt
    m_funcmap[0x252] = nullptr;    // SetProgramDir
    m_funcmap[0x258] = nullptr;    // GetProgramDir
    m_funcmap[0x25e] = nullptr;    // SystemTagList
    m_funcmap[0x25e] = nullptr;    // System
    m_funcmap[0x264] = nullptr;    // AssignLock
    m_funcmap[0x26a] = nullptr;    // AssignLate
    m_funcmap[0x270] = nullptr;    // AssignPath
    m_funcmap[0x276] = nullptr;    // AssignAdd
    m_funcmap[0x27c] = nullptr;    // RemAssignList
    m_funcmap[0x282] = nullptr;    // GetDeviceProc
    m_funcmap[0x288] = nullptr;    // FreeDeviceProc
    m_funcmap[0x28e] = nullptr;    // LockDosList
    m_funcmap[0x294] = nullptr;    // UnLockDosList
    m_funcmap[0x29a] = nullptr;    // AttemptLockDosList
    m_funcmap[0x2a0] = nullptr;    // RemDosEntry
    m_funcmap[0x2a6] = nullptr;    // AddDosEntry
    m_funcmap[0x2ac] = nullptr;    // FindDosEntry
    m_funcmap[0x2b2] = nullptr;    // NextDosEntry
    m_funcmap[0x2b8] = nullptr;    // MakeDosEntry
    m_funcmap[0x2be] = nullptr;    // FreeDosEntry
    m_funcmap[0x2c4] = nullptr;    // IsFileSystem
    m_funcmap[0x2ca] = nullptr;    // Format
    m_funcmap[0x2d0] = nullptr;    // Relabel
    m_funcmap[0x2d6] = nullptr;    // Inhibit
    m_funcmap[0x2dc] = nullptr;    // AddBuffers
    m_funcmap[0x2e2] = nullptr;    // CompareDates
    m_funcmap[0x2e8] = nullptr;    // DateToStr
    m_funcmap[0x2ee] = nullptr;    // StrToDate
    m_funcmap[0x2f4] = nullptr;    // InternalLoadSeg
    m_funcmap[0x2fa] = nullptr;    // InternalUnLoadSeg
    m_funcmap[0x300] = nullptr;    // NewLoadSeg
    m_funcmap[0x300] = nullptr;    // NewLoadSegTagList
    m_funcmap[0x306] = nullptr;    // AddSegment
    m_funcmap[0x30c] = nullptr;    // FindSegment
    m_funcmap[0x312] = nullptr;    // RemSegment
    m_funcmap[0x318] = nullptr;    // CheckSignal
    m_funcmap[0x31e] = nullptr;    // ReadArgs
    m_funcmap[0x324] = nullptr;    // FindArg
    m_funcmap[0x32a] = nullptr;    // ReadItem
    m_funcmap[0x330] = nullptr;    // StrToLong
    m_funcmap[0x336] = nullptr;    // MatchFirst
    m_funcmap[0x33c] = nullptr;    // MatchNext
    m_funcmap[0x342] = nullptr;    // MatchEnd
    m_funcmap[0x348] = nullptr;    // ParsePattern
    m_funcmap[0x34e] = nullptr;    // MatchPattern
    m_funcmap[0x35a] = nullptr;    // FreeArgs
    m_funcmap[0x366] = nullptr;    // FilePart
    m_funcmap[0x36c] = nullptr;    // PathPart
    m_funcmap[0x372] = nullptr;    // AddPart
    m_funcmap[0x378] = nullptr;    // StartNotify
    m_funcmap[0x37e] = nullptr;    // EndNotify
    m_funcmap[0x384] = nullptr;    // SetVar
    m_funcmap[0x38a] = nullptr;    // GetVar
    m_funcmap[0x390] = nullptr;    // DeleteVar
    m_funcmap[0x396] = nullptr;    // FindVar
    m_funcmap[0x3a2] = nullptr;    // CliInitNewcli
    m_funcmap[0x3a8] = nullptr;    // CliInitRun
    m_funcmap[0x3ae] = nullptr;    // WriteChars
    m_funcmap[0x3ba] = nullptr;    // VPrintf
    m_funcmap[0x3c6] = nullptr;    // ParsePatternNoCase
    m_funcmap[0x3cc] = nullptr;    // MatchPatternNoCase
    m_funcmap[0x3d8] = nullptr;    // SameDevice
    m_funcmap[0x3de] = nullptr;    // ExAllEnd
    m_funcmap[0x3e4] = nullptr;    // SetOwner

    // setup jump table (TRAP and RTS instructions for each routine)
    for (auto it = m_funcmap.begin(); it != m_funcmap.end(); ++it) {
        m68k_write_16(base - it->first, 0x4e40);
        m68k_write_16(base - it->first + 2, 0x4e75);
    }
}


void DOSLibrary::getFileInfo(const Poco::File &obj, struct FileInfoBlock *fib)
{
    // file / directory name
    strncpy(fib->fib_FileName, Poco::Path(obj.path()).getFileName().c_str(), 100);

    // type
    if (obj.isDirectory())
        fib->fib_DirEntryType = +1;
    else
        fib->fib_DirEntryType = -1;                         // We treat special files as regular files

    // size
    if (obj.isFile())
        fib->fib_Size = SWAP_BYTES(obj.getSize());         // This is only correct if the file is smaller than 4GB
    else
        // TODO: What was the size of directory in AmigaDOS?
        fib->fib_Size = 0;

    // flags (only the ones that are available in Unix / Windows)
    fib->fib_Protection = 0;
    if (!obj.canRead())
        fib->fib_Protection |= FIBF_READ;
    if (!obj.canWrite())
        fib->fib_Protection |= FIBF_WRITE;
    if (!obj.canExecute())
        fib->fib_Protection |= FIBF_EXECUTE;
    fib->fib_Protection = SWAP_BYTES(fib->fib_Protection);

    // timestamp
    int tzdiff;
    auto tsdiff = Poco::DateTime(obj.getLastModified()) - Poco::DateTimeParser::parse("%d/%m/%Y %H:%M:%S %Z", "01/01/1978 00:00:00 GMT", tzdiff);
    fib->fib_Date.ds_Days   = SWAP_BYTES(tsdiff.days());
    fib->fib_Date.ds_Minute = SWAP_BYTES(tsdiff.hours() * 60 + tsdiff.minutes());
    fib->fib_Date.ds_Tick   = SWAP_BYTES(tsdiff.seconds() * 50);
}


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
    std::cout.flush();
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
// returns: > 0 or 0 in case of an error
//
uint32_t DOSLibrary::Examine()
{
    LOG4CXX_DEBUG(g_logger, "DOSLibrary::Examine() has been called");
    struct FileLock *lock = (struct FileLock *) PTR_M68K_TO_HOST(PTR_BCPL_TO_C(m68k_get_reg(NULL, M68K_REG_D1)));
    struct FileInfoBlock *fib   = (struct FileInfoBlock *) PTR_M68K_TO_HOST(m68k_get_reg(NULL, M68K_REG_D2));

    Poco::File *obj = (Poco::File *) lock->fl_Key;
    if (obj->isDirectory()) {
        // create DirectoryIterator object and store the pointer in fl_Task field of the lock. This of course breaks
        // programs which use this field to find out the handler that owns the lock...
        Poco::DirectoryIterator *it = new Poco::DirectoryIterator(*obj);
        lock->fl_Task = (struct MsgPort *) it;
    }

    // fill FileInfoBlock with information of the current object
    getFileInfo(*obj, fib);
    return 1;
}


//
// ExNext
// D1: BPTR to struct FileLock
// D2: pointer to struct FileInfoBlock
// returns: > 0 or 0 in case of an error
//
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

    // fill FileInfoBlock with information of the current object (dereferencing the iterator provides a Poco::File object)
    LOG4CXX_DEBUG(g_logger, "current object: " << it->path().getFileName());
    getFileInfo(**it, fib);

    // move iterator to next entry
    ++(*it);
    return 1;
}


//
// IoErr
// returns: error from last system routine
//
uint32_t DOSLibrary::IoErr()
{
    LOG4CXX_DEBUG(g_logger, "DOSLibrary::IoErr() has been called");
    return m_errno;
}
