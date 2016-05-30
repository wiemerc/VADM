//
// minimalistic startup code to be used with the example programs
//

#include "proto/exec.h"

static char libname[] = "dos.library";
struct ExecBase *SysBase = (struct ExecBase *) 4;
struct DosLibrary *DOSBase;

void start()
{
    if ((DOSBase = (struct DosLibrary *) OpenLibrary(libname, 0L)) == NULL)
        return;

    cwmain();
}
