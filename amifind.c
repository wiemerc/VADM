#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <dos/dosextens.h>


#define MAX_DEPTH 16
#define MAX_PATH_LEN 1024


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


void search(const char *dir)
{
    BPTR                   lock;
    struct FileInfoBlock *fib;
    char                   newdir[MAX_PATH_LEN];
    static unsigned int    depth = 0;

    if ((lock = Lock (dir, ACCESS_READ)) != 0) {
        if ((fib = AllocVec(sizeof(struct FileInfoBlock), MEMF_CLEAR)) != NULL) {
            if (Examine(lock, fib)) {
                if (fib->fib_DirEntryType > 0) {
                    ++depth;
                    if (depth <= MAX_DEPTH) {
                        printf("examing directory '%s' (depth = %d)\n", dir, depth);
                        while (ExNext(lock, fib)) {
                            if(fib->fib_DirEntryType > 0) {
                                // another directory => call ourselves recursively
                                strncpy(newdir, dir, MAX_PATH_LEN - 1);
                                strncat(newdir, fib->fib_FileName, MAX_PATH_LEN - 1 - strlen(dir));
                                search(newdir);
                            }
                            else {
                                // plain file => just output file name and size for now
                                printf("%-30s%-ld\n", fib->fib_FileName, fib->fib_Size);
                            }
                        }
                        if (IoErr() != ERROR_NO_MORE_ENTRIES)
                            printf("error occurred while examing directory '%s': %ld\n", dir, IoErr());
                    }
                    else
                        printf("maximum recursion depth reached - aborting\n");
                }
                else
                    printf("search() was called on a file - aborting\n");
            }
            FreeVec(fib);
        }
        else
            printf("could not allocate memory for FileInfoBlock\n");

        UnLock(lock);
    }
    else
        printf("could not obtain lock for directory %s\n", dir);

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
