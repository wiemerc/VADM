//
// VADE - classes representing the different system libraries
//
// Copyright(C) 2016 Constantin Wiemer
//


#include "libs.h"
#include "memory.h"


// global logger
extern log4cxx::LoggerPtr g_logger;

// global pointer to memory
extern uint8_t *g_mem;

// global map of opened libraries
extern std::map <uint32_t, Library *> g_libmap;


//
// methods of Library
//
void Library::call(const uint16_t offset)
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

//
// OpenLibrary
// A1: library name
// D0: version (not used)
// returns: pointer to library or 0
//
uint32_t ExecLibrary::OpenLibrary()
{
    LOG4CXX_DEBUG(g_logger, "ExecLibrary::OpenLibrary() was called");
    const char *libname = (const char *) g_mem + m68k_get_reg(NULL, M68K_REG_A1);
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


uint32_t ExecLibrary::AllocMem()
{
    return 0;
}


uint32_t ExecLibrary::FreeMem()
{
    return 0;
}


uint32_t ExecLibrary::FindTask()
{
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
    LOG4CXX_DEBUG(g_logger, "DOSLibrary::PutStr() was called");
    const char *str = (const char *) g_mem + m68k_get_reg(NULL, M68K_REG_D1);
    std::cout << str;
    return 0;
}


//
// VPrintf
// D1: string
// D2: pointer to arguments
// returns: number of bytes written or -1
//
uint32_t DOSLibrary::VPrintf()
{
    LOG4CXX_DEBUG(g_logger, "DOSLibrary::VPrintf() was called");
    const char *str = (const char *) (g_mem + m68k_get_reg(NULL, M68K_REG_D1));
    uint32_t argp = m68k_get_reg(NULL, M68K_REG_D2);
    LOG4CXX_TRACE(g_logger, "string = '" << str << "'");

    const char *p = str;
    bool isfmt = false;
    while (*p != 0) {
        if (!isfmt) {
            if (*p == '%')
                isfmt = true;
            else
                std::cout << *p;
        }
        else {
            switch (*p) {
                case 'd':
                    std::cout << Poco::format("%d", (const int32_t) m68k_read_32(argp));
                    argp += 4;
                    break;
                case 's':
                    std::cout << (const char *) (g_mem + m68k_read_32(argp));
                    argp += 4;
                    break;
                case '%':
                    std::cout << *p;
                    break;
                default:
                    LOG4CXX_ERROR(g_logger, "invalid format specifier: " << *p);
                    return -1;
            }
            isfmt = false;
        }
        ++p;
    }
    return 0;
}


uint32_t DOSLibrary::IoErr()
{
    return 0;
}


uint32_t DOSLibrary::Lock()
{
    return 0;
}


uint32_t DOSLibrary::UnLock()
{
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

