// The string needs to be defined as global *array* (not pointer), otherwise GCC will put it at the
// *beginning* of the code section => maybe a bug in GCC?
static char s[] = "hello world";

void cwtest()
{
    char *p;
    for (p = s; *p != 0; p++) {
        if (*p >= 97 && *p <= 122)
            *p -= 32;
    }
    return;
}