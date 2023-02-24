KDB_TREE_SRC= includes/KdbTree.cpp
BKD_TREE_SRC= includes/BkdTree.cpp
MOCK_API_SRC= includes/MockAPI.cpp
MAIN_SRC= main.cpp

PROGRAM_SRC=$(MAIN_SRC) $(BKD_TREE_SRC) $(KDB_TREE_SRC) $(MOCK_API_SRC)

HEADERS=includes/Config.h includes/MemoryStructures.h includes/ThreadStructures.h includes/RCUContainers.h includes/BkdTree.h includes/KdbTree.h includes/MockAPI.h



all: program

program: $(PROGRAM_SRC) $(HEADERS) Makefile
	g++ $(PROGRAM_SRC) -g -lpthread -o $@ 


clean:
	rm -f *~ *.o *.exe *.stackdump 


# valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./program