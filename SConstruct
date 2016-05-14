# main program
env = Environment()
env.Replace(CC = 'clang', CXX = 'clang++')
env.Append(CCFLAGS = '-Wall -g')
env.Append(CXXFLAGS = '-Wall -g -I/usr/local/include -I/opt/m68k-amigaos/os-include')
env.Program ('vade', ['vade.cxx'] + Glob ('Musashi/*.c'), LIBS = ['log4cxx', 'PocoFoundation'], LIBPATH = '/usr/local/lib')

# test program
Command ('cwtest.o', 'cwtest.c', '/opt/m68k-amigaos/bin/m68k-amigaos-gcc -c -o cwtest.o cwtest.c')
Command ('cwtest', 'cwtest.o', '/opt/m68k-amigaos/bin/m68k-amigaos-ld -s -o cwtest cwtest.o')
