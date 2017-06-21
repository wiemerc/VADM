//
// VADM - class for loading the executable in Amiga Hunk format
//
// Copyright(C) 2016 Constantin Wiemer
//


#include "loader.h"


// global logger
extern log4cxx::LoggerPtr g_logger;

// global pointer to memory
extern uint8_t *g_mem;


void AmiHunkLoader::load(char *fname, uint32_t loc)
{
    Poco::FileInputStream exe(fname);
    Poco::BinaryReader::BinaryReader reader(exe, Poco::BinaryReader::BIG_ENDIAN_BYTE_ORDER);
    uint32_t btype;                                 // block type
    uint32_t hnum = 0;                              // hunk number
    uint32_t hloc = loc;                            // hunk location relative to the base address g_mem
    std::vector <uint32_t> hlocs;                   // mapping of hunk numbers to locations
    while (true) {
        reader >> btype;
        if (reader.eof())
            break;

        switch (btype)
        {
            case HUNK_HEADER:
                LOG4CXX_INFO(g_logger, "hunk #" << hnum << ", block type = HUNK_HEADER");
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
                    LOG4CXX_DEBUG(g_logger, "size (in bytes) of hunk #" << i << " = " << lword * 4 << ", location = " << Poco::format("0x%08x", hloc));
                    hlocs.push_back(hloc);
                    hloc += lword * 4;
                }
                break;

            case HUNK_CODE:
                LOG4CXX_INFO(g_logger, "hunk #" << hnum << ", block type = HUNK_CODE");
                uint32_t nwords;
                reader >> nwords;
                LOG4CXX_DEBUG(g_logger, "size (in bytes) of code block: " << nwords * 4);
                reader.readRaw((char *) g_mem + hlocs[hnum], nwords * 4);
                LOG4CXX_TRACE(g_logger, "hex dump of block:\n" << hexdump(g_mem + hlocs[hnum], nwords * 4));
                break;

            case HUNK_DATA:
                LOG4CXX_INFO(g_logger, "hunk #" << hnum << ", block type = HUNK_DATA");
                reader >> nwords;
                LOG4CXX_DEBUG(g_logger, "size (in bytes) of data block: " << nwords * 4);
                reader.readRaw((char *) g_mem + hlocs[hnum], nwords * 4);
                LOG4CXX_TRACE(g_logger, "hex dump of block:\n" << hexdump(g_mem + hlocs[hnum], nwords * 4));
                break;

            case HUNK_BSS:
                LOG4CXX_INFO(g_logger, "hunk #" << hnum << ", block type = HUNK_BSS");
                reader >> nwords;
                LOG4CXX_DEBUG(g_logger, "size (in bytes) of BSS block: " << nwords * 4);
                break;

            case HUNK_RELOC32:
                LOG4CXX_INFO(g_logger, "hunk #" << hnum << ", block type = HUNK_RELOC32");
                uint32_t noffsets;
                while (true) {
                    reader >> noffsets;
                    if (noffsets == 0)
                        break;

                    uint32_t refhnum;
                    reader >> refhnum;
                    if (hnum > lhunk) {
                        LOG4CXX_ERROR(g_logger, "reloc referring to hunk #" << refhnum << " found while executable contains only " << lhunk + 1 << " hunks");
                        throw std::runtime_error ("bad executable");
                    }

                    uint32_t  offset;
                    for (int i = 0; i < noffsets; i++) {
                        reader >> offset;
                        LOG4CXX_TRACE(g_logger, "applying reloc referring to hunk #" << refhnum << ", offset = " << offset);
                        m68k_write_32(hlocs[hnum] + offset, m68k_read_32(hlocs[hnum] + offset) + hlocs[refhnum]);
                    }
                }
                break;

            case HUNK_SYMBOL:
                LOG4CXX_INFO(g_logger, "hunk #" << hnum << ", block type = HUNK_SYMBOL");
                LOG4CXX_ERROR(g_logger, "block type is not implemented");
                throw std::runtime_error ("block type not implemented");
                break;

            case HUNK_END:
                LOG4CXX_INFO(g_logger, "hunk #" << hnum << ", block type = HUNK_END");
                ++hnum;
                break;

            default:
                LOG4CXX_ERROR(g_logger, "unknown block type: " << btype);
        }
    }
}

