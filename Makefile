CXXFILES = $(wildcard *.cxx)
CFILES = $(wildcard Musashi/*.c)
OBJS = $(patsubst %.cxx, %.o, $(CXXFILES))
OBJS += $(patsubst %.c, %.o, $(CFILES))

CC = clang
CFLAGS = -m32 -Wall -g
CXX = clang++
CXXFLAGS = -m32 -Wall -g -I/opt/local/include -I/usr/local/include -I/opt/m68k-amigaos/os-include
LDFLAGS = -arch i386 -L/opt/local/lib -L/usr/local/lib
LDLIBS = -llog4cxx -lPocoFoundation

vadm: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.cxx
	$(CXX) $(CXXFLAGS) -c $< -o $@

