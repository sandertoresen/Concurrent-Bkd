#ifndef MEMORY_STRUCTURES_H
#define MEMORY_STRUCTURES_H
#include <atomic>
#include <list>
#include "Config.h"

using namespace std;

struct DataNode
{
    float cordinates[DIMENSIONS];
    char location[CHARACTER_LIMIT];
};

struct AtomicTreeElement
{
    // readers flag
    long treeId; // id for writers to locate variable
    atomic<int> readers = 0;
    atomic<bool> deleted = false;
    KdbTree *tree;
};

struct AtomicUnorderedMapElement
{
    atomic<int> readers = 0;
    atomic<bool> deleted = false;
    unordered_map<long, AtomicTreeElement *> *readableTrees;
};

#endif