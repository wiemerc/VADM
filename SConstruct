# main program
main = Environment()
main.Replace(CC = 'clang', CXX = 'clang++')
main.Append(CCFLAGS = '-Wall -g')
main.Append(CXXFLAGS = '-Wall -g -I/usr/local/include -I/opt/m68k-amigaos/os-include')
main.Program('vade', Glob('*.cxx') + Glob ('Musashi/*.c'), LIBS = ['log4cxx', 'PocoFoundation'], LIBPATH = '/usr/local/lib')

# test programs
test = Environment()
test.Replace(CC = '/opt/m68k-amigaos/bin/m68k-amigaos-gcc')
test.Append(CCFLAGS = '-Wall -I/usr/include -D__KLIBC__')
test.Append(LINKFLAGS = '-s -nostdlib')
test.Program('strtoupper', ['strtoupper.c'])
test.Program('amihello', ['cwcrt0.c', 'amihello.c'])
test.Program('amifind', ['cwcrt0.c', 'amifind.c'] + Glob('klibc/*.c'))
