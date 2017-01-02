CXXFILES = $(wildcard *.cxx)
OBJS = $(patsubst %.cxx, %.o, $(CXXFILES))

CXX = clang++
CXXFLAGS = -m32 -Wall -Wno-c++11-extensions -g -I/opt/local/include -I/usr/local/include -I/opt/m68k-amigaos/os-include
LDFLAGS = -arch i386 -L/opt/local/lib -L/usr/local/lib
LDLIBS = -llog4cxx -lPocoFoundation

.PHONY: clean Musashi Examples

vadm: Musashi $(OBJS) Examples
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) Musashi/*.o $(LDLIBS)

clean:
	$(MAKE) --directory=Musashi clean
	$(MAKE) --directory=Examples clean
	rm -f *.o vadm

Musashi:
	$(MAKE) --directory=$@

Examples:
	$(MAKE) --directory=$@

%.o: %.cxx
	$(CXX) $(CXXFLAGS) -c $< -o $@

