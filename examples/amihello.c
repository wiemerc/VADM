#include "proto/dos.h"

// We must not name the function main, otherwise GCC will insert a call to __main which does apparently some
// initializations for the standard library.
int cwmain(int argc, char **argv)
{
    PutStr("hello, amiga\n");
    Exit(0);
    return 0;
}
