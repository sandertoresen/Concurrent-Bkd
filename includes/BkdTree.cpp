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
    globalMemory = new DataNode[GLOBAL_BUFFER_SIZE];
    globalMemorySize = 0;

    fill_n(globalChunkReady, GLOBAL_B_CHUNK_SIZE, false);
    globalDisk = NULL;
    globalDiskSize = -1;
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

// Thread should spin and fetch datapoints from an API until it ThreadDataNodeBuffer is full
// Then atomically insert it into BkdTree
// if Global is full, thread is responsible for creating new global memory buffer
// and managing tree up to a sertain size
/*
struct input {
    BkdTree *tree;
    InsertApi *stream; // mock stream of data
};
*/
void *_threadInserter(void *bkdTree)
{
    BkdTree *tree = (BkdTree *)bkdTree;

    DataNode threadData[THREAD_BUFFER_SIZE];

    for (int i = 0; i < THREAD_BUFFER_SIZE; i++)
    {
        threadData[i].cordinates[0] = 20.5f;
        threadData[i].cordinates[1] = 63.5f;
        strcpy(threadData[i].location, "Input");
    }

    int size, updatedSize;

    while (1)
    {
        size = tree->globalMemorySize.load();
        updatedSize = size + THREAD_BUFFER_SIZE;

        if (size >= GLOBAL_BUFFER_SIZE)
            continue;
        if (tree->globalMemorySize.compare_exchange_weak(size, updatedSize))
            break;
    }

    memcpy(&tree->globalMemory[size],
           &threadData, sizeof(DataNode) * THREAD_BUFFER_SIZE);

    printf("Size: %d Set value at chunk %d\n", size, size / THREAD_BUFFER_SIZE);
    tree->globalChunkReady[size / THREAD_BUFFER_SIZE] = true;

    // Tree now full -> handle data
    if (updatedSize < GLOBAL_BUFFER_SIZE)
    {
        pthread_exit(NULL);
    }

    bool moreWork = true;
    while (moreWork)
    {
        moreWork = false;
        for (int i = 0; i < GLOBAL_B_CHUNK_SIZE; i++)
        {
            moreWork += !tree->globalChunkReady[i];
        }
    }

    if (tree->globalDisk == NULL)
    {
        tree->globalDisk = tree->globalMemory;
        tree->globalDiskSize.store(GLOBAL_BUFFER_SIZE);

        fill_n(tree->globalChunkReady, GLOBAL_B_CHUNK_SIZE, false);

        tree->globalMemory = new DataNode[GLOBAL_BUFFER_SIZE];
        tree->globalMemorySize.store(0);
        printf("Used globalDisk\n");
        pthread_exit(NULL);
    }

    printf("Obs.. not implemented yet, bulkloading\n");
    pthread_exit(NULL);
}
