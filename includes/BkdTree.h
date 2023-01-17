#ifndef BKD_TREE_H
#define BKD_TREE_H
#include <list>
#include <cstdarg>
#include "Config.h"
#include "MemoryStructures.h"

class BkdTree
{
public:
    BkdTree();
    ~BkdTree();

    int insert(DataNode *value);
    int remove(float index[DIMENSIONS]);

private:
    GlobalMemoryDataNodeBuffer *globalMemory;
    GlobalMemoryDataNodeBuffer *globalDisk;
};

#endif