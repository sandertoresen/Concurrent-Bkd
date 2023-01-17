#include <iostream>
#include <list>
#include <string.h>
#include <algorithm>
#include <cmath>
#include "Config.h"
#include "BkdTree.h"

BkdTree::BkdTree() // default constructor
{
    printf("Constructur runs!\n");
    globalMemory = new GlobalMemoryDataNodeBuffer;
    globalDisk = NULL;
}

BkdTree::~BkdTree() // Destructor
{
    printf("Destructor runs!\n");
    if (globalMemory != NULL)
    {
        delete globalMemory;
    }

    if (globalDisk != NULL)
    {
        delete globalDisk;
    }
}
