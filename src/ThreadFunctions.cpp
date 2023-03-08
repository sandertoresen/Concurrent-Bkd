#include <iostream>
#include <list>
#include <string.h>
#include <atomic>
#include "headers/Config.h"
#include "headers/BkdTree.h"
#include "headers/MockAPI.h"
#include "headers/ThreadFunctions.h"
#include "headers/KdbTree.h"
#include "headers/scheduler.h"

using namespace std;
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
        // threadData[i].cordinates[0] = (float)i;
        // threadData[i].cordinates[1] = (float)i;
        // strcpy(threadData[i].location, "Input");
        tree->API->fetchRandom(&threadData[i]);
    }
    int inserts = 0;
    while (inserts > THREAD_BUFFER_SIZE)
    { // Scheduler needs to assign APIs to a given thread
      // MVP: thread recives list of indexes its responsible for
      // APINodeArray[index]
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
        pthread_exit(nullptr);
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

    if (tree->globalDisk == nullptr)
    {
        tree->globalDisk = tree->globalMemory;
        tree->globalDiskSize.store(GLOBAL_BUFFER_SIZE);

        fill_n(tree->globalChunkReady, GLOBAL_B_CHUNK_SIZE, false);

        tree->globalMemory = new DataNode[GLOBAL_BUFFER_SIZE];
        tree->globalMemorySize.store(0);
        printf("Used globalDisk\n");

        pthread_exit(nullptr);
    }

    printf("-----------Start bulkload\n");
    // clear tree->globalMemory and globalDisk
    // put old globalMemory and globalDisk into Read variable untill the data has been inserted (RCU)

    tree->_bulkloadTree();

    pthread_exit(nullptr);
}

void *_threadInserterApi(SchWriterThread *input)
{
    SchWriterThread *in = (SchWriterThread *)input;
    BkdTree *tree = in->tree;
    DataNode threadData[THREAD_BUFFER_SIZE];

    int inserts = 0;
    while (inserts < THREAD_BUFFER_SIZE)
    { // TODO: threads can recive array of datanodes
        // Scheduler needs to assign APIs to a given thread
        // MVP: thread recives list of indexes its responsible for
        // APINodeArray[index]
        for (int i = 0; i < API_MAX_WRITER; i++)
        {
            if (in->nodes[i] == nullptr)
            {
                continue;
            }
            // TODO: assert threadsafe schedulerVSthread
            APIWriteNode *api = in->nodes[i].load();

            if (api->containsDataFlag.load() == 1)
            {
                threadData[inserts++] = api->value;
                api->containsDataFlag.store(-1);
                api->wait--;
                if (inserts + 1 == THREAD_BUFFER_SIZE)
                    break;
            }
        }
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
        pthread_exit(nullptr);
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

    if (tree->globalDisk == nullptr)
    {
        tree->globalDisk = tree->globalMemory;
        tree->globalDiskSize.store(GLOBAL_BUFFER_SIZE);

        fill_n(tree->globalChunkReady, GLOBAL_B_CHUNK_SIZE, false);

        tree->globalMemory = new DataNode[GLOBAL_BUFFER_SIZE];
        tree->globalMemorySize.store(0);
        printf("Used globalDisk\n");

        pthread_exit(nullptr);
    }

    printf("-----------Start bulkload\n");
    // clear tree->globalMemory and globalDisk
    // put old globalMemory and globalDisk into Read variable untill the data has been inserted (RCU)

    tree->_bulkloadTree();

    pthread_exit(nullptr);
}
void *_windowLookup(void *input)
{
    WindowLookupInput *windowIn = (WindowLookupInput *)input;
    BkdTree *bkdTree = windowIn->tree;
    if (bkdTree->globalReadMap == nullptr)
    {
        pthread_exit(nullptr);
    }

    // get local pointer to data
    AtomicUnorderedMapElement *localMap = bkdTree->globalReadMap;

    localMap->readers++;
    for (auto it = localMap->readableTrees->begin(); it != localMap->readableTrees->end(); it++)
    {
        AtomicTreeElement *tmp = it->second;
        KdbTreeRangeSearch(tmp->tree, windowIn->window, windowIn->results);
    }

    localMap->readers--;
    pthread_exit(nullptr);
}