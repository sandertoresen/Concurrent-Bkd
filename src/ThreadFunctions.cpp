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

        if (updatedSize < GLOBAL_BUFFER_SIZE)
        {
            continue;
        }

        bool moreWork = true;
        // Wait for other threads
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

void *_threadInserterTree(void *writerThread)
{
    chrono::time_point<chrono::high_resolution_clock> testTotalTimeStart;
    chrono::time_point<chrono::high_resolution_clock> testTotalTimeEnd;

    ScheduledThread *thread = (ScheduledThread *)writerThread;
    BkdTree *tree = thread->tree;
    if (thread->threadId == 0)
    {
        testTotalTimeStart = chrono::high_resolution_clock::now();
    }
    int totalNodes = 4096 * 4096;
    int numTrees = thread->numTrees;
    int added = 0;
    int treeSize = totalNodes / numTrees;
    while (thread->flag.load() == 1)
    {
        DataNode *threadData = new DataNode[treeSize * DIMENSIONS];

        for (int i = 0; i < treeSize; i++)
        {
            tree->API->fetchRandom(&threadData[i]);
        }
        tree->_smallBulkloadTree(threadData, treeSize);
        added++;
        if (added == numTrees)
        {
            printf("added: %d\n", added);
            break;
        }
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
        WindowQuery *query = tree->API->fetchWindowQuery();
        auto start = chrono::high_resolution_clock::now();

        AtomicUnorderedMapElement *localMap = tree->globalReadMap.load();

        if (localMap == nullptr)
        {
            continue;
        }
        // printf("My new epoch: %d\n", localMap->epoch.load());
        thread->epoch.store(localMap->epoch);

        for (const auto &[treeId, kdbTree] : *localMap->readableTrees)
        {
            // printf("Reading tree: %d|%d\n", kdbTree->id, treeId);
            KdbTreeRangeSearch(kdbTree, query->window, query->results);
        }

        auto stop = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::duration<double>>(stop - start);

        printf("Read %d values in %fs from %d trees\n", query->results.size(), duration.count(), localMap->readableTrees->size());
        exit(0);
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
        // printf("From %d trees|time:%fs|\n\n", localMap->readableTrees->size(), duration.count());
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
        int maxLevel = 0; // sch->bkdTree->largestLevel.load();
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