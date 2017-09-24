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


#include <stdio.h>
#include <stdint.h>
#include <proto/exec.h>
#include <proto/dos.h>


// The library name needs to be defined as global *array* (not pointer), otherwise GCC
// will treat it as a constant and put it at the *beginning* of the code block, which is
// a problem because there is no start address in the Hunk format (unlike ELF or PE).
static char libname[] = "dos.library";

struct ExecBase *SysBase;
struct DosLibrary *DOSBase;
FILE *stdout;
FILE *stdin;

int cwmain(int argc, char **argv);


void start()
{
    // push argc (D0) / argv (A0) onto stack
    asm("move.l     A0, -(A7)\n"
        "move.l     D0, -(A7)\n");

    // initialize library symbols
    SysBase = *((struct ExecBase **) 0x00000004);
    if ((DOSBase = (struct DosLibrary *) OpenLibrary(libname, 0L)) == NULL)
        return;

    // initialize stdout and stdin
    if ((stdin = (FILE *) AllocVec(sizeof(FILE), MEMF_CLEAR)) == NULL)
        return;
    stdin->_fileno = (int32_t) Input();
    if ((stdout = (FILE *) AllocVec(sizeof(FILE), MEMF_CLEAR)) == NULL)
        return;
    stdout->_fileno = (int32_t) Output();

    // call cwmain()
    asm("jsr        _cwmain\n"
        "add.l      #8, A7");
}
