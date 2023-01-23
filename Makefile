# KDB_TREE_SRC=includes/KDB_Tree.cpp
MEMORY_STRUCTURES_SRC= includes/MemoryStructures.cpp
BKD_TREE_SRC= includes/BkdTree.cpp
MAIN_SRC= main.cpp

PROGRAM_SRC=$(MAIN_SRC) $(MEMORY_STRUCTURES_SRC) $(BKD_TREE_SRC) #$(KDB_TREE_SRC)

HEADERS=includes/Config.h includes/MemoryStructures.h includes/BkdTree.h



all: program

# indexer: $(INDEXER_SRC) $(HEADERS) Makefile
# 	gcc -Wall -o $@ -D_GNU_SOURCE -D_REENTRANT $(INDEXER_SRC) -g -lpthread -lm

program: $(PROGRAM_SRC) $(HEADERS) Makefile
	g++ $(PROGRAM_SRC) -g -lpthread -o $@ 

# assert_index: $(ASSERT_SRC) $(HEADERS) Makefile
# 	gcc -o $@ $(ASSERT_SRC) -g -lm

clean:
	rm -f *~ *.o *.exe *.stackdump 


# Using built-in specs.
# COLLECT_GCC=/usr/bin/g++
# COLLECT_LTO_WRAPPER=/usr/lib/gcc/x86_64-pc-msys/10.2.0/lto-wrapper.exe
# Target: x86_64-pc-msys
# Configured with: /c/_/S/gcc/src/gcc-10.2.0/configure --build=x86_64-pc-msys --prefix=/usr --libexecdir=/usr/lib --enable-bootstrap --enable-shared --enable-shared-libgcc --enable-static --enable-version-specific-runtime-libs --with-arch=x86-64 --with-tune=generic --disable-multilib --enable-__cxa_atexit --with-dwarf2 --enable-languages=c,c++,fortran,lto --enable-graphite --enable-threads=posix --enable-libatomic --enable-libgomp --disable-libitm --enable-libquadmath --enable-libquadmath-support --disable-libssp --disable-win32-registry --disable-symvers --with-gnu-ld --with-gnu-as --disable-isl-version-check --enable-checking=release --without-libiconv-prefix --without-libintl-prefix --with-system-zlib --enable-linker-build-id --with-default-libstdcxx-abi=gcc4-compatible --enable-libstdcxx-filesystem-ts
# Thread model: posix
# Supported LTO compression algorithms: zlib
# gcc version 10.2.0 (GCC)