KDB_TREE_SRC= src/KdbTree.cpp
BKD_TREE_SRC= src/BkdTree.cpp
THREAD_FUNCTIONS_SRC = src/ThreadFunctions.cpp
MOCK_API_SRC= src/MockAPI.cpp
SCHEDULER_SRC= src/scheduler.cpp

TEST_SRC= src/tests.cpp
MAIN_SRC= main.cpp

PROGRAM_SRC=$(MAIN_SRC) $(BKD_TREE_SRC) $(KDB_TREE_SRC) $(MOCK_API_SRC) $(TEST_SRC) $(THREAD_FUNCTIONS_SRC) $(SCHEDULER_SRC)

HEADERS=src/headers/Config.h src/headers/MemoryStructures.h src/headers/ThreadFunctions.h src/headers/BkdTree.h src/headers/KdbTree.h src/headers/MockAPI.h src/headers/tests.h src/headers/scheduler.h



all: program

program: $(PROGRAM_SRC) $(HEADERS) Makefile
	g++ $(PROGRAM_SRC) -g -lpthread -o $@ 


clean:
	rm -f *~ *.o *.exe *.stackdump 


# valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./program