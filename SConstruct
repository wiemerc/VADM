# main program
main = Environment(TARGET_ARCH = 'i386')
main.Replace(CC = 'clang', CXX = 'clang++')
main.Append(CCFLAGS = '-m32 -Wall -g')
main.Append(CXXFLAGS = '-m32 -Wall -g -I/opt/local/include -I/usr/local/include -I/opt/m68k-amigaos/os-include')
main.Append(LINKFLAGS = '-arch i386')
main.Program('vadm', Glob('*.cxx') + Glob ('Musashi/*.c'), LIBS = ['log4cxx', 'PocoFoundation'], LIBPATH = ['/opt/local/lib', '/usr/local/lib'])

# test programs
test = Environment()
test.Replace(CC = '/opt/m68k-amigaos/bin/m68k-amigaos-gcc')
test.Append(CCFLAGS = '-Wall -Ilinux-usr-include -Ilinux-usr-include/i386-linux-gnu -D__KLIBC__')
test.Append(LINKFLAGS = '-s -nostdlib')
test.Program('strtoupper', ['strtoupper.c'])
test.Program('amihello', ['cwcrt0.c', 'amihello.c'])
test.Program('amifind', ['cwcrt0.c', 'amifind.c'] + Glob('klibc/*.c'))
