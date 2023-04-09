#include <atomic>
#include <chrono>
#include <iostream>
#include <list>
#include <string.h>
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

void *_threadInserter(void *writerThread)
{
    ScheduledThread *thread = (ScheduledThread *)writerThread;
    BkdTree *tree = thread->tree;
    while (thread->flag.load() == 1)
    {
        DataNode threadData[THREAD_BUFFER_SIZE];

        for (int i = 0; i < THREAD_BUFFER_SIZE; i++)
        {
            tree->API->fetchRandom(&threadData[i]);
        }

        int size, updatedSize;

        while (1)
        {
            size = tree->globalMemorySize.load();
            updatedSize = size + THREAD_BUFFER_SIZE;

            if (size >= GLOBAL_BUFFER_SIZE)
                continue;
            if (tree->globalMemorySize.compare_exchange_strong(size, updatedSize))
                break;
        }

        memcpy(&tree->globalMemory[size],
               &threadData, sizeof(DataNode) * THREAD_BUFFER_SIZE);

        // printf("Size: %d Set value at chunk %d\n", size, size / THREAD_BUFFER_SIZE);
        tree->globalChunkReady[size / THREAD_BUFFER_SIZE] = true;

        // Tree now full -> handle data
        if (updatedSize < GLOBAL_BUFFER_SIZE)
        {
            continue;
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

            continue;
        }

        // clear tree->globalMemory and globalDisk
        // put old globalMemory and globalDisk into Read variable untill the data has been inserted (RCU)
        tree->_bulkloadTree();
    }
    thread->flag.store(-1);
    pthread_exit(nullptr);
}

void *_windowLookup(void *readerThread)
{

    ScheduledThread *thread = (ScheduledThread *)readerThread;

    BkdTree *tree = thread->tree;

    while (thread->flag.load() == 1)
    {
        auto start = chrono::high_resolution_clock::now();
        WindowQuery *query = tree->API->fetchWindowQuery();

        // get local pointer to data
        AtomicUnorderedMapElement *localMap = tree->globalReadMap;
        if (localMap == nullptr)
        {
            continue;
        }
        localMap->readers++;
        for (auto it = localMap->readableTrees->begin(); it != localMap->readableTrees->end(); it++)
        {
            KdbTree *tmp = it->second;
            KdbTreeRangeSearch(tmp, query->window, query->results);
        }
        localMap->readers--;

        auto stop = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::duration<double>>(stop - start);

        // printf("Read %d values in %fs\n", query->results.size(), duration.count());
        // TODO do something with results(?)
        for (auto it = query->results.begin(); it != query->results.end();)
        {
            DataNode tmp = *it;
            if (tree->isDeleted(tmp.location))
            {
                it = query->results.erase(it);
            }
            else
            {
                it++;
            }
        }

        // printf("Got %d size query\n", query->results.size());
        query->results.clear();

        delete query;
    }
    thread->flag.store(-1);
    pthread_exit(nullptr);
}

void *_performLargerBulkLoad(void *bulkLoadThread)
{
    BulkLoadThread *thread = (BulkLoadThread *)bulkLoadThread;
    Scheduler *sch = thread->scheduler;
    while (thread->flag.load() == 1)
    {
        int maxLevel = sch->bkdTree->largestLevel.load();
        for (int i = 0; i <= maxLevel; i++)
        {
            if (thread->flag.load() != 1)
                break;

            sch->largeBulkloads(i);
        }
    }
    thread->flag.store(-1);

    pthread_exit(nullptr);
}