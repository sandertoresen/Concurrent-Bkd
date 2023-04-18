#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <pthread.h>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include "headers/BkdTree.h"
#include "headers/KdbTree.h"
#include "headers/MockAPI.h"
#include "headers/scheduler.h"
#include "string.h"

using namespace std;

Scheduler::Scheduler()
{
    // TODO Setup
    bkdTree = new BkdTree;
}
Scheduler::~Scheduler()
{
    if (writers.begin() != writers.end())
    {
        printf("OBS writers not deleted!\n");
    }
    if (readers.begin() != readers.end())
    {
        printf("OBS readers not deleted!\n");
    }

    if (bulkThread != nullptr)
    {
        printf("OBS bulkThread not deleted!\n");
    }

    delete bkdTree;
}

void *_schedulerMainThread(void *scheduler)
{
    Scheduler *sch = (Scheduler *)scheduler;

    bool running = true;
    // spawn initial writer threads
    for (int i = 0; i < INITIAL_WRITERS; i++)
    {
        ScheduledThread *thread = new ScheduledThread;
        thread->flag = 1;
        thread->tree = sch->bkdTree;
        pthread_create(&thread->thread, nullptr, _threadInserter, (void *)thread);
        sch->activeThreads++;
        sch->writers.push_back(thread);
    }

    for (int i = 0; i < INITIAL_READERS; i++)
    {
        ScheduledThread *thread = new ScheduledThread;
        thread->flag = 1;
        thread->tree = sch->bkdTree;
        thread->epoch = 0;
        pthread_create(&thread->thread, nullptr, _windowLookup, (void *)thread);
        sch->activeThreads++;
        sch->readers.push_back(thread);
    }

    if (BULK_THREAD)
    {
        sch->bulkThread = new BulkLoadThread;
        sch->bulkThread->flag = 1;
        sch->bulkThread->scheduler = sch;
        pthread_create(&sch->bulkThread->thread, nullptr, _performLargerBulkLoad, (void *)sch->bulkThread);
    }
    while (running)
    {
        int currentCount = sch->bkdTree->treeId.load();

        if (sch->bkdTree->treeId.load() > TREES_CREATED)
        {
            printf("Got %d trees\n", sch->bkdTree->treeId.load());
            sch->shutdown();
            break;
        }

        while (currentCount + EPOCH_WAIT_NUM > sch->bkdTree->treeId.load())
        {
            sched_yield();
        }
        sch->deleteOldMaps();
    }
    // check performance and if more threads are needed(?)

    // perform later bulkloadings

    // cleanup old trees/scheduler maps..

    // Simulate workflow by changing API->delay
    return nullptr;
}

void Scheduler::deleteOldMaps()
{
    // schedulerDeletedMaps[i]
    // bkdTree->schedulerDeletedMaps

    // printf("epoch: %d\n", smallestEpoch);

    for (int i = 0; i < SCHEDULER_MAP_ARRAY_SIZE; i++)
    {
        long smallestEpoch = readers.empty() ? -1 : readers.front()->epoch.load();
        for (auto thread : readers)
        {
            long localReaderEpoch = thread->epoch.load();
            smallestEpoch = localReaderEpoch < smallestEpoch ? localReaderEpoch : smallestEpoch;
        }
        AtomicUnorderedMapElement *deleteMap = bkdTree->schedulerDeletedMaps[i].load();
        if (deleteMap == nullptr)
        {
            continue;
        }
        if (deleteMap->epoch >= smallestEpoch && smallestEpoch != -1)
        {
            continue;
        }

        // For each map here delete them if flagged as deleted and have no readers..
        //  deleteMap->readableTrees
        // Next store values based on epoch
        if (deleteMap->oldTrees != nullptr)
        {
            for (auto oldTree : *deleteMap->oldTrees)
            {
                // printf("Call delete on tree %d size: %d\n", oldTree->id, oldTree->size);
                KdbDestroyTree(oldTree);
            }
        }
        delete deleteMap->oldTrees;
        delete deleteMap->readableTrees;
        delete deleteMap;

        bkdTree->schedulerDeletedMaps[i].store(nullptr);
    }
}

void Scheduler::shutdown()
{
    for (auto it = writers.begin(); it != writers.end(); it++)
    {
        printf("Shut down write thread\n");
        ScheduledThread *t = *it;
        t->flag.store(0);
    }

    for (auto it = readers.begin(); it != readers.end(); it++)
    {
        printf("Shut down read thread\n");
        ScheduledThread *t = *it;
        t->flag.store(0);
    }

    // TODO: shutdown large bulkload thread
    if (bulkThread != nullptr)
    {
        printf("Shut down bulkThread\n");
        bulkThread->flag.store(0);
    }

    for (auto it = writers.begin(); it != writers.end();)
    {
        ScheduledThread *t = *it;
        while (t->flag.load() != -1)
        {
        }
        pthread_join(t->thread, nullptr);
        delete t;
        it = writers.erase(it);
    }

    for (auto it = readers.begin(); it != readers.end();)
    {
        ScheduledThread *t = *it;
        while (t->flag.load() != -1)
        {
        }
        pthread_join(t->thread, nullptr);
        delete t;
        it = readers.erase(it);
    }
    if (bulkThread != nullptr)
    {
        while (bulkThread->flag.load() != -1)
        {
        }
        pthread_join(bulkThread->thread, nullptr);
        delete bulkThread;
        bulkThread = nullptr;
    }
    printf("threads erased!\n");
}

void Scheduler::largeBulkloads(int selectedLevel)
{
    // TODO/FILL: include numTrees to know how many trees of selected level should be bulkloaded(?)

    int numNodes = 0;
    list<KdbTree *> *mergeTreeList = new list<KdbTree *>;
    if (selectedLevel == 0)
    {
        pthread_mutex_lock(&bkdTree->mediumWriteTreesLock);
        int size = bkdTree->globalWriteMediumTrees.size();
        if (size < LARGE_BULKLOAD_SIZE)
        {
            pthread_mutex_unlock(&bkdTree->mediumWriteTreesLock);
            delete mergeTreeList;
            return;
        }

        int numNodesAdded = 0;
        auto it = bkdTree->globalWriteMediumTrees.begin();

        while (it != bkdTree->globalWriteMediumTrees.end())
        {
            KdbTree *tmp = *it;
            mergeTreeList->push_back(tmp);
            it = bkdTree->globalWriteMediumTrees.erase(it); // Erase the value and update the iterator
            numNodes = tmp->size;
            numNodesAdded++;
            if (numNodesAdded == LARGE_BULKLOAD_SIZE)
            {
                break;
            }
        }
        pthread_mutex_unlock(&bkdTree->mediumWriteTreesLock);
    }
    else
    {
        printf("I am running larges bulkload!!\n\n\n");
        int numNodesAdded = 0;
        for (auto it = bkdTree->globalWriteLargeTrees.begin(); it != bkdTree->globalWriteLargeTrees.end(); it++)
        {
            KdbTree *tmp = *it;
            if (tmp->level == selectedLevel)
            {
                mergeTreeList->push_back(tmp);
                numNodes += tmp->size;
                numNodesAdded++;

                if (numNodesAdded == LARGE_BULKLOAD_SIZE)
                {
                    break;
                }
            }
        }
        if (numNodesAdded != LARGE_BULKLOAD_SIZE)
        {
            delete mergeTreeList;
            return;
        }

        for (auto deletedTree : *mergeTreeList)
        {
            bkdTree->globalWriteLargeTrees.remove(deletedTree);
        }
    }
    printf("Nr nodes: %d\n", numNodes);
    DataNode *bloomValues = new DataNode[numNodes * DIMENSIONS];
    int offset = 0;
    for (auto kdbTree : *mergeTreeList)
    {
        KdbTreeFetchNodes(kdbTree, &bloomValues[offset]);
        offset += kdbTree->size;
    }

    int updatedNodes = numNodes;
    for (int i = 0; i < numNodes; i++)
    {
        if (bkdTree->isDeleted(bloomValues[i].location))
        {
            bloomValues[i].location[0] = '\0'; // Mark the node as deleted
            updatedNodes--;
        }
    }

    // Create new DataNode *array without any deleted nodes
    printf("Large bulkload nr of values %d, total allcoate:%d\n", updatedNodes, updatedNodes * DIMENSIONS);
    DataNode *values = new DataNode[updatedNodes * DIMENSIONS];
    int j = 0;
    for (int i = 0; i < numNodes; i++)
    {
        if (bloomValues[i].location[0] != '\0')
        {
            values[j] = bloomValues[i];
            j++;
        }
    }

    delete[] bloomValues;
    numNodes = updatedNodes;

    for (int d = 0; d < DIMENSIONS; d++)
    {
        if (d != 0) // paste over tree data
            memcpy(&values[d * numNodes], &values[0], sizeof(DataNode) * numNodes);

        sort(&values[d * numNodes], &values[d * numNodes + numNodes], dataNodeCMP(d));
    }

    int level = selectedLevel ? mergeTreeList->size() * selectedLevel : mergeTreeList->size();
    printf("level is: %d based on selectedLevel(%d) * list size(%d)\n", level, selectedLevel, mergeTreeList->size());
    if (level > bkdTree->largestLevel.load())
    {
        bkdTree->largestLevel.store(level);
    }

    printf("Started LARGER BULKLOAD of %d nodes!\n", numNodes);
    KdbTree *tree = KdbCreateTree(values, numNodes, generateUniqueId(bkdTree->treeId), level);

    // remove old nodes
    delete[] values;

    bkdTree->globalWriteLargeTrees.push_back(tree);
    bkdTree->updateReadTrees(mergeTreeList, tree);
    printf("Completed large bulkload\n");
}