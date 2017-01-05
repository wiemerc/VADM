# VADM - Virtual Amiga DOS Machine

Copyright(C) 2016, 2017 Constantin Wiemer

VADM executes (simple) programs written and compiled for the Amiga microcomputer running Amiga OS on Linux, macOS and probably also on Windows (not tested though). This is just a proof-of-concept of an ABI emulator which I wrote for educational purposes and fun. Therefore only a very small part of the actual Amiga DOS / Amiga OS is emulated and it will for sure contain bugs. It was inspired by Wine, the virtual DOS machine (VDM) in Windows NT and Wabi for Solaris. For the emulation of the CPU (Motorola M680x0) the code of the _Musashi_ emulator is used. In addition, the example programs use parts of _klibc_.

To execute a program, you call the emulator with the name of the program as argument, and then optionally any arguments for the program itself, for example like this:  
```
./vadm examples/amifind examples -name "*.c"
```

## Building
You need to have **32-bit** versions of [POCO](https://pocoproject.org) and [log4cxx](https://logging.apache.org/log4cxx/latest_stable/) installed. This is because the emulator will always be built as 32-bit binary, even if the platform is 64 bits. As the Amiga was a 32-bit computer, it was just easier this way instead of converting between 32 and 64 bits everywhere in the code.

In order to compile _klibc_ on macOS or Windows, you need to put a copy of `/usr/include` from a Linux system into `Examples/linux-usr-include`. On Linux you just need to change `linux-usr-include` to `/usr/include` in the Makefiles.

* Download or clone the repository
* Change into the directory where you put it
* Type `make`. This will download and patch the necessary parts of _klibc_ and _Musashi_, generate the _Musashi_ code and compile everything. After `make` is finished, you will find the executable `vadm` in the current directory. Alternatively, you can do the same using [SCons](scons.org) by typing `scons`, but of course only if you have _SCons_ installed.

