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

    if (bulkLoader != nullptr)
    {
        printf("OBS bulkloader not deleted!\n");
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
        pthread_create(&thread->thread, nullptr, _windowLookup, (void *)thread);
        sch->activeThreads++;
        sch->readers.push_back(thread);
    }

    if (BULK_THREAD)
    {
        BulkLoadThread *bulkThread = new BulkLoadThread;
        bulkThread->flag = 1;
        bulkThread->scheduler = sch;
        pthread_create(&bulkThread->thread, nullptr, _performLargerBulkLoad, (void *)bulkThread);
    }

    while (running)
    {
        if (sch->bkdTree->treeId.load() > TREES_CREATED)
        {
            printf("Got %d trees\n", sch->bkdTree->treeId.load());
            sch->shutdown();
            break;
        }

        sch->deleteOldMaps();
    }
    // check performance and if more threads are needed(?)

    // perform later bulkloadings

    // cleanup old trees/scheduler maps..

    // Simulate workflow by changing API->delay
    return NULL;
}

void Scheduler::deleteOldMaps()
{
    // schedulerDeletedMaps[i]
    // bkdTree->schedulerDeletedMaps

    for (int i = 0; i < SCHEDULER_MAP_ARRAY_SIZE; i++)
    {
        AtomicUnorderedMapElement *deleteMap = bkdTree->schedulerDeletedMaps[i].load();
        if (deleteMap == nullptr)
        {
            continue;
        }
        if (deleteMap->readers.load() != 0)
        {
            continue;
        }

        // For each map here delete them if flagged as deleted and have no readers..
        //  deleteMap->readableTrees
        for (const auto &[treeId, kdbTree] : *deleteMap->readableTrees)
        {
            if (!kdbTree->deleted.load())
            {
                continue;
            }

            if (deletedKdbTrees.find(treeId) == deletedKdbTrees.end())
            { // OBS used kdbTree->id earlier here
                printf("Call delete on tree %d size: %d\n", treeId, kdbTree->size);

                KdbDestroyTree(kdbTree);
                deletedKdbTrees.insert(treeId);
            }

            bkdTree->schedulerDeletedMaps[i].store(nullptr);
        }
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
    if (bulkLoader != nullptr)
    {
        printf("Shut down bulkloader\n");
        bulkLoader->flag.store(0);
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
    if (bulkLoader != nullptr)
    {
        while (bulkLoader->flag.load() != -1)
        {
        }
        pthread_join(bulkLoader->thread, nullptr);
        delete bulkLoader;
        bulkLoader = nullptr;
    }
    printf("threads erased!\n");
}

void Scheduler::largeBulkloads(int selectedLevel)
{
    // TODO/FILL: include numTrees to know how many trees of selected level should be bulkloaded(?)

    int numNodes = 0;
    list<KdbTree *> mergeTreeList;
    if (selectedLevel == 0)
    {
        pthread_mutex_lock(&bkdTree->mediumWriteTreesLock);
        int size = bkdTree->globalWriteMediumTrees.size();
        int largestPowerOf2 = pow(2, floor(log2(size))); // largest power of 2 less than or equal to size
        if (largestPowerOf2 < 2)
        {
            pthread_mutex_unlock(&bkdTree->mediumWriteTreesLock);
            return;
        }
        int numNodesAdded = 0;
        auto it = bkdTree->globalWriteMediumTrees.begin();
        while (it != bkdTree->globalWriteMediumTrees.end() && numNodesAdded < largestPowerOf2)
        {
            KdbTree *tmp = *it;
            mergeTreeList.push_back(tmp);
            it = bkdTree->globalWriteMediumTrees.erase(it); // Erase the value and update the iterator
            numNodesAdded++;
            numNodes = tmp->size;
        }
        pthread_mutex_unlock(&bkdTree->mediumWriteTreesLock);
    }
    else
    {
        int treeCount = 0;
        for (auto it = bkdTree->globalWriteLargeTrees.begin(); it != bkdTree->globalWriteLargeTrees.end(); it++)
        {
            KdbTree *tmp = *it;
            if (tmp->level == selectedLevel)
            {
                mergeTreeList.push_back(tmp);
                numNodes += tmp->size;
                treeCount++;
            }
        }
        int size = mergeTreeList.size();
        int largestPowerOf2 = pow(2, floor(log2(size)));
        if (largestPowerOf2 < 2)
        {
            return;
        }
        while (size > largestPowerOf2)
        {
            KdbTree *tmp = mergeTreeList.back();
            mergeTreeList.pop_back();
            numNodes -= tmp->size;
            size--;
        }
    }
    DataNode *bloomValues = new DataNode[numNodes * DIMENSIONS];
    int offset = 0;
    for (auto treePtr : mergeTreeList)
    {
        KdbTreeFetchNodes(treePtr, &bloomValues[offset]);
        offset += treePtr->size;
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

        std::sort(&values[d * numNodes], &values[d * numNodes + numNodes], dataNodeCMP(d));
    }

    int level = selectedLevel ? mergeTreeList.size() * selectedLevel : mergeTreeList.size();
    if (level > bkdTree->largestLevel.load())
    {
        bkdTree->largestLevel.store(level);
    }

    printf("Started LARGER BULKLOAD of %d nodes!\n", numNodes);
    KdbTree *tree = KdbCreateTree(values, numNodes, generateUniqueId(bkdTree->treeId), level);

    // remove old nodes
    delete[] values;

    bkdTree->globalWriteLargeTrees.push_back(tree);

    AtomicUnorderedMapElement *mapCopy = new AtomicUnorderedMapElement;
    pthread_mutex_lock(&bkdTree->globalReadMapWriteLock);
    if (bkdTree->globalReadMap == nullptr)
    {
        mapCopy->readableTrees = new unordered_map<long, KdbTree *>();
    }
    else
    {
        mapCopy->readableTrees = new unordered_map<long,
                                                   KdbTree *>(*bkdTree->globalReadMap->readableTrees);
    }

    // exit(1);
    // for value in map: if value.id exists in mergeTreeList, delete it
    for (auto it = mergeTreeList.begin(); it != mergeTreeList.end();)
    {
        KdbTree *deleteTree = *it;
        printf("Deleting %ld from map copy\n", deleteTree->id);
        mapCopy->readableTrees->erase(deleteTree->id);
        if (bkdTree->globalReadMap != NULL)
        {
            unordered_map<long, KdbTree *> *mapPtr = bkdTree->globalReadMap->readableTrees;

            auto itr = mapPtr->find(deleteTree->id);
            if (itr != mapPtr->end())
            {
                KdbTree *treeContainer = itr->second;
                treeContainer->deleted.store(true);
            }
            else
            {
                // OBS didnt find tree to delete??
                printf("|large bulkload|DIDN'T FIND TREE TO DELETE SHOULDNT BE POSSIBLE\n");
                printf("Didn't find tree %ld\n", deleteTree->id);
                exit(-1);
            }
        }

        it = mergeTreeList.erase(it);
    }
    mapCopy->readableTrees->insert(make_pair(tree->id, tree));

    AtomicUnorderedMapElement *oldMap = bkdTree->globalReadMap;
    bkdTree->globalReadMap = mapCopy;
    pthread_mutex_unlock(&bkdTree->globalReadMapWriteLock);

    if (oldMap != nullptr)
    {
        oldMap->deleted.store(true);
        while (true)
        {
            int index = -1;
            for (int i = 0; i < SCHEDULER_MAP_ARRAY_SIZE; i++)
            {
                AtomicUnorderedMapElement *ptr = bkdTree->schedulerDeletedMaps[i].load();
                if (ptr == nullptr)
                {
                    index = i;
                    break;
                }
            }
            AtomicUnorderedMapElement *expected = nullptr;
            if (index != -1 && bkdTree->schedulerDeletedMaps[index].compare_exchange_strong(expected, oldMap))
            {
                break;
            }
            else if (index == -1)
            {
                printf("ERROR ARRAY IS FULL BECAUSE OF LACK OF SCHEDULED!\n\n\n");
                exit(-1);
            }
        }
    }
    printf("Completed large bulkload\n");
}