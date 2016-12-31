OBJS = __divsi3.o __modsi3.o __udivdi3.o __udivmoddi4.o __udivmodsi4.o __umoddi3.o

CC = /opt/m68k-amigaos/bin/m68k-amigaos-gcc
CFLAGS = -Wall -g -I../linux-usr-include -I../linux-usr-include/i386-linux-gnu -D__KLIBC__
LDFLAGS = -s -nostdlib

libgcc: $(OBJS)
	touch libgcc

%.c: %.c.patch
	wget -q https://raw.githubusercontent.com/brainflux/klibc/master/usr/klibc/libgcc/$@
	patch -p0 < $@.patch

%.c:
	wget -q https://raw.githubusercontent.com/brainflux/klibc/master/usr/klibc/libgcc/$@

