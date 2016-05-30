// The string needs to be defined as global *array* (not pointer), otherwise GCC will treat it as a constant and
// put it at the *beginning* of the code block.
static char s[] = "hello world";

void strtoupper()
{
    char *p;
    for (p = s; *p != 0; p++) {
        if (*p >= 97 && *p <= 122)
            *p -= 32;
    }
    return;
}