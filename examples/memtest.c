//
// memtest.c - test program for the memory manager
//


#include <proto/exec.h>
#include <proto/dos.h>


int cwmain()
{
    void *ptr1, *ptr2;

    // allocate a block than can hold 2 x 256 bytes and the MCB for the second block
    PutStr("allocating 520 bytes\n");
    ptr1 = AllocVec(520, 0);
    FreeVec(ptr1);

    // check if size is rounded up to 256 and previous block is split
    PutStr("allocating 100 bytes\n");
    ptr1 = AllocVec(100, 0);
    FreeVec(ptr1);

    // check if both parts of the split block are reused
    PutStr("allocating 2 x 256 bytes\n");
    ptr1 = AllocVec(256, 0);
    ptr2 = AllocVec(256, 0);
    FreeVec(ptr2);
    FreeVec(ptr1);

    return 0;
}
