CFILES = $(wildcard klibc/*.c)
OBJS = $(patsubst %.c, %.o, $(CFILES))

CC = /opt/m68k-amigaos/bin/m68k-amigaos-gcc
CFLAGS = -Wall -g -Ilinux-usr-include -Ilinux-usr-include/i386-linux-gnu -D__KLIBC__
LDFLAGS = -s -nostdlib

all: strtoupper amihello amifind memtest

strtoupper: strtoupper.o
	$(CC) $(LDFLAGS) -o $@ $^

amihello: cwcrt0.o amihello.o
	$(CC) $(LDFLAGS) -o $@ $^

amifind: cwcrt0.o amifind.o $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

memtest: cwcrt0.o memtest.o
	$(CC) $(LDFLAGS) -o $@ $^


