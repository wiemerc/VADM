OBJS := memset.o snprintf.o strchr.o strcmp.o strlen.o strncat.o strncpy.o printf.o vfprintf.o vsnprintf.o

CC     := /opt/m68k-amigaos/bin/m68k-amigaos-gcc
CFLAGS := -Wall -g -I../include -I../include/i386-linux-gnu -D__KLIBC__

klibc: $(OBJS)
	touch klibc

clean:
	rm -f *.o klibc

%.c: %.c.patch
	wget -q https://raw.githubusercontent.com/brainflux/klibc/master/usr/klibc/$@
	patch -p0 < $@.patch

%.c:
	wget -q https://raw.githubusercontent.com/brainflux/klibc/master/usr/klibc/$@

