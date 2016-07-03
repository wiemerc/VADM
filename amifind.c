#include <stdio.h>
#include <stdarg.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <dos/dosextens.h>


int printf(const char *str, ...)
{
    va_list argp;
    char buffer[1024];

    va_start(argp, str);
    vsnprintf(buffer, 1024, str, argp);
    PutStr(buffer);
    va_end(argp);
    return 0;
}


void search(const char *path)
{
    BPTR lock;
    struct FileInfoBlock *fib;

    if ((lock = Lock (path, ACCESS_READ)) != 0) {
        printf("directory key = %ld\n", ((struct FileLock *) BADDR(lock))->fl_Key);

        if ((fib = AllocVec(sizeof(struct FileInfoBlock), MEMF_CLEAR)) != NULL) {
            if (Examine(lock, fib)) {
                if (fib->fib_DirEntryType > 0) {
                    printf("examing directory '%s'", path);
                    while (ExNext(lock, fib)) {
                        if(fib->fib_DirEntryType > 0) {
                            // another directory => call ourself recursively
                            search(fib->fib_FileName);
                        }
                        else {
                            // plain file => just output file name and size for now
                            printf("%-40s%ld\n", fib->fib_FileName, fib->fib_Size);
                        }
                    }
                    if (IoErr() != ERROR_NO_MORE_ENTRIES)
                        printf("error occurred while examing directory '%s': %ld\n", path, IoErr());
                }
                else {
                    printf("search() was called on a file - aborting\n");
                }
            }
            FreeVec(fib);
        }
        else {
            printf("could not allocate memory for FileInfoBlock\n");
        }
        UnLock(lock);
    }
    else {
        printf("could not obtain lock for directory %s\n", path);
    }
    return;
}


int cwmain(int argc, char **argv)
{
    char *dir, *arg, *pattern = "", *flags = "", *mtime = "";

    printf("argc = %d\n", argc);
    printf("program name = %s\n", *argv);

    if (*++argv != NULL) {
        dir = *argv;
        printf("dir = %s\n", dir);
    
        while (*++argv != NULL) {
            if (**argv == '-') {
                arg = *argv;
                printf("arg = %s\n", arg);
                if (*++argv != NULL) {
                    if (strcmp(arg, "-name") == 0)
                        pattern = *argv;
                    else if (strcmp(arg, "-flags") == 0)
                        flags = *argv;
                    else if (strcmp(arg, "-mtime") == 0)
                        mtime = *argv;
                    else
                        printf("unknown predicate '%s'\n", arg);
                }
                else
                    break;
            }
            
        }
    }
    else {
        printf("usage: amifind <dir> [-name <pattern>] [-flags <flags>] [-mtime <[+|-]days]\n");
        return 1;
    }
    
    printf("pattern = '%s'\n", pattern);
    printf("flags = '%s'\n", flags);
    printf("mtime = '%s'\n", mtime);

    search(dir);

    return 0;
}
