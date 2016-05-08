#generator = Program ('m68kmake.c')
#gencode = Command ('m68kops.h', 'm68k_in.c', ['./m68kmake'])
#Depends (gencode, generator)

# main program
env = Environment()
env.Append(CCFLAGS = '-Wall -g')
env.Program ('vade', ['vade.c'] + Glob ('Musashi/*.c'))

# test program
Command ('cwtest.o', 'cwtest.s', '/opt/m68k-amigaos/bin/m68k-amigaos-as -o cwtest.o cwtest.s')
Command ('cwtest', 'cwtest.o', '/opt/m68k-amigaos/bin/m68k-amigaos-ld -o cwtest cwtest.o')
