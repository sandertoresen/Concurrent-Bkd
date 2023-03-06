#include <iostream>
#include <list>
#include <string.h>
#include <atomic>
#include "headers/Config.h"
#include "headers/BkdTree.h"
#include "headers/MockAPI.h"
#include "headers/ThreadFunctions.h"
#include "headers/KdbTree.h"

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
    windowLookupInput *windowIn = (windowLookupInput *)input;
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

void *_MockAPIHandler(void *APIs)
{
    // TODO both read and writes dice roll which one
}

void *_MockAPIWrite(void *APIs)
{
    // TODO store avg wait time and use that as a unit instead(?)
    MockApi *API = (MockApi *)API;
    for (int i = 0; i < API_WRITERS; i++)
    {
        APIWriteNode *api = &API->APINodeArray[i];
        int flag = api->flag.load();

        if (flag == 1)
            continue;

        if (flag == -1)
            api->wait--;

        int counter = API->mockDataPtr.fetch_add(1);

        api->value = &API->mockData[counter];
        api->flag.store(1);
        api->wait++;
        if (api->flag.load() != 0)
            continue;
        // DataNode tmpVal =

        // API->APINodeArray[i].value =
        // set sent data flag
        atomic<int> flag;
        flag.store(1);

        // set as waiting
        atomic<int> wait = 0;
        wait++;
        while (flag.load() != 0)
            pthread_yield();

        wait--;

        // APIArray->mockData
    }
}