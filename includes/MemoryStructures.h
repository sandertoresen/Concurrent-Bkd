#ifndef MEMORY_STRUCTURES_H
#define MEMORY_STRUCTURES_H
#include <atomic>
#include "Config.h"

using namespace std;

struct DataNode
{
    float cordinates[DIMENSIONS];
    char *location[CHARACTER_LIMIT];
};

// struct KDB_Leaf
// {
//     DataNode LeafArray[KDB_LEAF_SIZE];
// };

struct ThreadDataNodeBuffer
{
    DataNode buffer[THREAD_BUFFER_SIZE];
    int size = 0;
};

struct GlobalMemoryDataNodeBuffer
{
    ThreadDataNodeBuffer threadDataNodeBuffers[GLOBAL_BUFFER_SIZE];
    atomic<int> globalSize;
};

#endif