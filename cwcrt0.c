//
// minimalistic startup code to be used with the example programs
// We use our own startup code for the two following reasons:
// (1) The calling convention for programs is different from the one on the Amiga (see comment in vadm.cxx)
// (2) Real startup codes do all kinds of fancy stuff, like initializing data structures for the C standard library.
//     This is a problem because it would require us to implement (possibly quite a lot of) additional library
//     routines. However, this also means that we cannot use a complete C standard library (because of the missing
//     initialization) but have to add the code for the necessary routines (in our case from klibc) to the project
//     and link it to the example programs directly.
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
