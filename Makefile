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

Poco:
	wget -q https://pocoproject.org/releases/poco-1.7.8/poco-1.7.8p2.tar.gz
	tar -xzf poco-1.7.8p2.tar.gz
	cd poco-1.7.8p2 && ./configure --config=Darwin32
	make -C poco-1.7.8p2
	make -C poco-1.7.8p2 install

%.o: %.cxx
	$(CXX) $(CXXFLAGS) -c $< -o $@

