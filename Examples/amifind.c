#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <dos/dos.h>
#include <dos/dosextens.h>


#define MAX_DEPTH 16
#define MAX_PATH_LEN 1024


//
// fnmatch - matches a file name against a pattern (a '?' matches a single character, a '*' matches an arbitrary string)
//
// returns: 1 if the string matches the pattern, 0 otherwise
//
int fnmatch(const char *pattern, const char *fname)
{
    const char *p = pattern;
    const char *s = fname;
    int			matched = 0;
    int         done    = 0;

    // shortcut if pattern is empty
    if (*p == 0)
        return 1;

    while ((*p != 0) && (*s != 0) && !done)
    {
        switch (*p)
        {
            case '?':
                 // matches a single character => we just move on to the next character
                ++p;
                ++s;
                break;

            case '*':
                 // matches an arbitrary but not empty string (at least one character)
                 // We move on to the next character in the pattern and then call fnmatch() recursively starting with
                 // the next character in the string and go through the string until we get a match or reach
                 // the end of the string.
                ++p;
                do
                {
                    ++s;
                    if (fnmatch(p, s))
                    {
                        matched = 1;
                        break;
                    }
                }
                while (*s != '\0');
                done = 1;
                break;

            default:
                 // plain character (includes NUL byte)
                 // If the characters match we just move on to the next character, otherwise we're done
                if (*p == *s)
                {
                    ++p;
                    ++s;
                }
                else
                    done = 1;
                break;
        }
    }
    if (!done && (*p == 0) && (*s == 0))
        matched = 1;

    return matched;
}


int flgmatch(const char *flagstr, const int flagmask)
{
    int matched = 1;

    while (matched && (*flagstr != 0)) {
        switch (*flagstr) {
            case 'r':
                matched = !(flagmask & FIBF_READ);
                break;
            case 'w':
                matched = !(flagmask & FIBF_WRITE);
                break;
            case 'x':
                matched = !(flagmask & FIBF_EXECUTE);
                break;
        }
        ++flagstr;
    }
    return matched;
}


void fmtdate(const struct DateStamp ds, char *buffer, const int bufsize)
{
    long minutes;

    minutes = ds.ds_Minute % 60;
    if (ds.ds_Tick > 30 * 50)
        ++minutes;
    snprintf(buffer, bufsize, "%ld days, %ld hours, %ld minutes", ds.ds_Days, ds.ds_Minute / 60, minutes);
}


void search(const char *dir, const char *pattern, const char *flagstr)
{
    BPTR                   lock;
    struct FileInfoBlock *fib;
    char                   newdir[MAX_PATH_LEN], date[50];
    static unsigned int    depth = 0;

    if ((lock = Lock (dir, ACCESS_READ)) == 0) {
        printf("could not obtain lock for directory %s\n", dir);
        goto ENOLOCK;
    }
    if ((fib = AllocVec(sizeof(struct FileInfoBlock), MEMF_CLEAR)) == NULL) {
        printf("could not allocate memory for FileInfoBlock\n");
        goto ENOMEM;
    }
    if (!Examine(lock, fib)) {
        if (IoErr() != ERROR_NO_MORE_ENTRIES)
            printf("error occurred while examing directory '%s': %ld\n", dir, IoErr());
        goto ENOEXAM;
    }
    if (fib->fib_DirEntryType <= 0) {
        printf("search() was called on a file - aborting\n");
        goto ENODIR;
    }
    
    ++depth;
    if (depth <= MAX_DEPTH) {
        printf("examing directory '%s' (depth = %d)\n", dir, depth);
        while (ExNext(lock, fib)) {
            if(fib->fib_DirEntryType > 0) {
                // another directory => call ourselves recursively
                strncpy(newdir, dir, MAX_PATH_LEN - 1);
                strncat(newdir, "/", MAX_PATH_LEN - 1 - strlen(dir));
                strncat(newdir, fib->fib_FileName, MAX_PATH_LEN - 2 - strlen(dir));
                search(newdir, pattern, flagstr);
            }
            else {
                // plain file => just output file name, size and flags if name and flags match
                if (fnmatch(pattern, fib->fib_FileName) && flgmatch(flagstr, fib->fib_Protection)) {
                    fmtdate(fib->fib_Date, date, 50);
                    printf("%s/%-30s%10ld\t%5ld\t%s\n", dir, fib->fib_FileName, fib->fib_Size,
                           fib->fib_Protection, date);
                }
            }
        }
        if (IoErr() != ERROR_NO_MORE_ENTRIES)
            printf("error occurred while examing directory '%s': %ld\n", dir, IoErr());
    }
    else
        printf("maximum recursion depth reached - aborting\n");
    --depth;
        
ENODIR:
ENOEXAM:
    FreeVec(fib);
ENOMEM:
    UnLock(lock);
ENOLOCK:
    return;
}


int cwmain(int argc, char **argv)
{
    char *dir, *arg, *pattern = "", *flags = "";

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
                    else
                        printf("unknown predicate '%s'\n", arg);
                }
                else
                    break;
            }
            
        }
    }
    else {
        printf("usage: amifind <dir> [-name <pattern>] [-flags <flags>]\n");
        return 1;
    }
    
    printf("pattern = '%s'\n", pattern);
    printf("flags = '%s'\n", flags);

    search(dir, pattern, flags);

    return 0;
}
