//
// minimalistic startup code to be used with the example programs
//


#include "proto/exec.h"


static char libname[] = "dos.library";
static struct ExecBase **AbsExecBase = (struct ExecBase **) 4;
struct ExecBase *SysBase;
struct DosLibrary *DOSBase;


int cwmain(int argc, char **argv);


void start()
{
    // push argc (D0) / argv (A0) onto stack
    asm("move.l     A0, -(A7)\n"
        "move.l     D0, -(A7)\n");

    SysBase = *AbsExecBase;
    if ((DOSBase = (struct DosLibrary *) OpenLibrary(libname, 0L)) == NULL)
        return;

    // call cwmain()
    asm("jsr        _cwmain\n"
        "add.l      #8, A7");
}
