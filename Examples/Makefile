CC = /opt/m68k-amigaos/bin/m68k-amigaos-gcc
CFLAGS = -Wall -g -Ilinux-usr-include -Ilinux-usr-include/i386-linux-gnu -D__KLIBC__
LDFLAGS = -s -nostdlib

.PHONY: all clean klibc libgcc

all: klibc libgcc strtoupper amihello amifind memtest

clean:
	$(MAKE) --directory=klibc clean
	$(MAKE) --directory=libgcc clean
	rm -f *.o strtoupper amihello amifind memtest

klibc libgcc:
	$(MAKE) --directory=$@

strtoupper: strtoupper.o

amihello: cwcrt0.o amihello.o
	$(CC) $(LDFLAGS) -o $@ cwcrt0.o $@.o

amifind: cwcrt0.o amifind.o klibc libgcc
	$(CC) $(LDFLAGS) -o $@ cwcrt0.o $@.o klibc/*.o libgcc/*.o

memtest: cwcrt0.o memtest.o
	$(CC) $(LDFLAGS) -o $@ cwcrt0.o $@.o

