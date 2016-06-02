//
// minimalistic startup code to be used with the example programs
//

#include "proto/exec.h"

static char libname[] = "dos.library";
struct ExecBase **AbsExecBase = (struct ExecBase **) 4;
struct ExecBase *SysBase;
struct DosLibrary *DOSBase;

void start()
{
    SysBase = *AbsExecBase;
    if ((DOSBase = (struct DosLibrary *) OpenLibrary(libname, 0L)) == NULL)
        return;

    cwmain();
}
