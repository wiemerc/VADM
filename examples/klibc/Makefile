OBJS = memset.o snprintf.o strchr.o strcmp.o strlen.o strncat.o strncpy.o vsnprintf.o

CC = /opt/m68k-amigaos/bin/m68k-amigaos-gcc
CFLAGS = -Wall -g -I../linux-usr-include -I../linux-usr-include/i386-linux-gnu -D__KLIBC__
LDFLAGS = -s -nostdlib

klibc: $(OBJS)
	touch klibc

%.c: %.c.patch
	wget -q https://raw.githubusercontent.com/brainflux/klibc/master/usr/klibc/$@
	patch -p0 < $@.patch

%.c:
	wget -q https://raw.githubusercontent.com/brainflux/klibc/master/usr/klibc/$@

