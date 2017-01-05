import os

env = Environment(CXX       = 'clang++',
                  CXXFLAGS  = '-std=c++11 -m32 -Wall -I/opt/local/include -I/usr/local/include -I/opt/m68k-amigaos/os-include ' + os.getenv('CXXFLAGS', ''),
                  LINKFLAGS = '-arch i386',
                  LIBS      = ['log4cxx', 'PocoFoundation'],
                  LIBPATH   = ['/opt/local/lib', '/usr/local/lib'])

SConscript(['Musashi/SConscript', 'Examples/SConscript'])

env.Program('vadm', Glob('*.cxx') + Split('Musashi/m68kcpu.o Musashi/m68kdasm.o Musashi/m68kopac.o Musashi/m68kopdm.o Musashi/m68kopnz.o Musashi/m68kops.o'))
