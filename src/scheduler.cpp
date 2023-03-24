#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <pthread.h>
#include <iostream>
#include <unordered_map>
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
        pthread_create(&thread->thread, nullptr, _threadInserterControlled, (void *)thread);
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

    BulkLoadThread *bulkThread = new BulkLoadThread;
    bulkThread->flag = 1;
    bulkThread->scheduler = sch;
    pthread_create(&bulkThread->thread, nullptr, _performLargerBulkLoad, (void *)bulkThread);
    while (running)
    {

        if (sch->bkdTree->treeId.load() > 1000)
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

    pthread_exit(nullptr);
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
        for (const auto &[_, value] : *deleteMap->readableTrees)
        {
            if (!value->deleted)
            {
                continue;
            }
            KdbDestroyTree(value->tree);

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
        it = writers.erase(it);
        // delete t;
    }

    for (auto it = readers.begin(); it != readers.end();)
    {
        ScheduledThread *t = *it;
        while (t->flag.load() != -1)
        {
        }
        it = readers.erase(it);
        // delete t;
    }
    if (bulkLoader != nullptr)
    {
        while (bulkLoader->flag.load() != -1)
        {
        }
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
        int size = bkdTree->globalWriteMediumTrees.size();
        int largestPowerOf2 = pow(2, floor(log2(size))); // largest power of 2 less than or equal to size
        if (largestPowerOf2 < 2)
        {
            return;
        }
        int numNodesAdded = 0;
        for (auto it = bkdTree->globalWriteMediumTrees.begin(); it != bkdTree->globalWriteMediumTrees.end() && numNodesAdded < largestPowerOf2; it++)
        {
            KdbTree *tmp = *it;
            mergeTreeList.push_back(tmp);
            numNodesAdded++;
            numNodes = tmp->size;
        }
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

    // TODO HERE: iterate over all fetched nodes and remove deleted nodes from bloomfilter
    // KISS:
    /*DataNode *bloomValues = new DataNode[numNodes];
    int offset = 0;
    for (auto itr = mergeTreeList.begin(); itr != mergeTreeList.end(); ++itr)
    {
        KdbTree *treePtr = *itr;
        KdbTreeFetchNodes(treePtr, &bloomValues[offset]);
        offset += treePtr->size;
    }
    for (int i = 0; i < numNodes; i++)
    {
        //check bloomfilter with val: bloomValues[i]
        //if bloomFilter:
            //if real:
                //delete node
                //numNodes--
    }
    //DataNode *values = bloomValues without deleted values
    //OBS old numNodes:
    for (int i = 0; i < numNodes; i++)
    {
        // if(bloomValues[i] != deleted)
            //value[seperateCount] = bloomValues[i]
    }*/

    DataNode *values = new DataNode[numNodes * DIMENSIONS];

    int offset = 0;
    for (auto itr = mergeTreeList.begin(); itr != mergeTreeList.end(); ++itr)
    {
        KdbTree *treePtr = *itr;
        KdbTreeFetchNodes(treePtr, &values[offset]);
        offset += treePtr->size;
    }

    // Here check for deleted values..

    for (int d = 0; d < DIMENSIONS; d++)
    {
        if (d != 0) // paste over tree data
            memcpy(&values[d * numNodes], &values[0], sizeof(DataNode) * numNodes);

        std::sort(&values[d * numNodes], &values[d * numNodes + numNodes], dataNodeCMP(d));
    }

    int level = selectedLevel ? mergeTreeList.size() * selectedLevel : mergeTreeList.size();

    KdbTree *tree = KdbCreateTree(values, numNodes, generateUniqueId(bkdTree->treeId), level);

    // remove old nodes
    delete[] values;

    bkdTree->globalWriteLargeTrees.push_back(tree);

    AtomicUnorderedMapElement *mapCopy = new AtomicUnorderedMapElement;

    if (bkdTree->globalReadMap == nullptr)
    {
        mapCopy->readableTrees = new unordered_map<long, AtomicTreeElement *>();
    }
    else
    {
        mapCopy->readableTrees = new unordered_map<long,
                                                   AtomicTreeElement *>(*bkdTree->globalReadMap->readableTrees);
    }

    // for value in map: if value.id exists in mergeTreeList, delete it
    for (auto it = mergeTreeList.begin(); it != mergeTreeList.end();)
    {
        KdbTree *deleteTree = *it;

        mapCopy->readableTrees->erase(deleteTree->id);

        if (bkdTree->globalReadMap != NULL)
        {
            unordered_map<long, AtomicTreeElement *> *mapPtr = bkdTree->globalReadMap->readableTrees;

            auto itr = mapPtr->find(deleteTree->id);
            if (itr != mapPtr->end())
            {
                AtomicTreeElement *treeContainer = itr->second;
                treeContainer->deleted.store(true);
            }
            else
            {
                // OBS didnt find tree to delete??
                printf("|large bulkload|DIDN'T FIND TREE TO DELETE SHOULDNT BE POSSIBLE\n");
                exit(-1);
            }
        }

        it = mergeTreeList.erase(it);
    }
    AtomicTreeElement *treeContainer = new AtomicTreeElement;
    treeContainer->tree = tree;
    treeContainer->treeId = tree->id;
    mapCopy->readableTrees->insert(make_pair(tree->id, treeContainer));

    AtomicUnorderedMapElement *oldMap = bkdTree->globalReadMap;
    bkdTree->globalReadMap = mapCopy;

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
}