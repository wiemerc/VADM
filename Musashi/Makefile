HEADERS = m68k.h m68kconf.h m68kcpu.h
OBJECTS = m68kcpu.o m68kdasm.o m68kopac.o m68kopdm.o m68kopnz.o m68kops.o

CC = clang
CFLAGS = -m32 -Wall -Wno-implicit-function-declaration -g


musashi: m68kmake $(HEADERS) $(OBJECTS)
	touch $@

clean:
	rm -f *.c *.h *.o m68kmake musashi

m68kmake: m68kmake.c m68k_in.c
	$(CC) -Wno-tautological-pointer-compare -o $@ $<
	./$@

%.c: %.c.patch
	wget -q https://raw.githubusercontent.com/kstenerud/Musashi/master/$@
	patch -p0 < $@.patch

%.c:
	wget -q https://raw.githubusercontent.com/kstenerud/Musashi/master/$@

%.h: %.h.patch
	wget -q https://raw.githubusercontent.com/kstenerud/Musashi/master/$@
	patch -p0 < $@.patch

%.h:
	wget -q https://raw.githubusercontent.com/kstenerud/Musashi/master/$@
