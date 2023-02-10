KDB_TREE_SRC= includes/KdbTree.cpp
MEMORY_STRUCTURES_SRC= includes/MemoryStructures.cpp
BKD_TREE_SRC= includes/BkdTree.cpp
MOCK_API_SRC= includes/MockAPI.cpp
MAIN_SRC= main.cpp

PROGRAM_SRC=$(MAIN_SRC) $(MEMORY_STRUCTURES_SRC) $(BKD_TREE_SRC) $(KDB_TREE_SRC) $(MOCK_API_SRC)

HEADERS=includes/Config.h includes/MemoryStructures.h includes/BkdTree.h includes/KdbTree.h includes/MockAPI.h



all: program

program: $(PROGRAM_SRC) $(HEADERS) Makefile
	g++ $(PROGRAM_SRC) -g -lpthread -o $@ 


clean:
	rm -f *~ *.o *.exe *.stackdump 
