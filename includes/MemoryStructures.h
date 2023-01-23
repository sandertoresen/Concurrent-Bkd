#ifndef MEMORY_STRUCTURES_H
#define MEMORY_STRUCTURES_H
#include <atomic>
#include "Config.h"

using namespace std;

struct DataNode
{
    float cordinates[DIMENSIONS];
    char location[CHARACTER_LIMIT];
};

// struct KDB_Leaf
// {
//     DataNode LeafArray[KDB_LEAF_SIZE];
// };

// struct GlobalMemoryDataNodeBuffer
// {
//     DataNode globalMemoryDataNodes[GLOBAL_BUFFER_SIZE];
//     atomic<int> globalSize;
// };

#endif