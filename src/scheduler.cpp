#include <algorithm>
#include <atomic>
#include <chrono>
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
    while (running)
    {

        if (sch->bkdTree->treeId.load() > 40)
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
    printf("threads erased!\n");
}

void Scheduler::largeBulkloads()
{
    // TODO: include bloomfilter

    // choose a nr of trees to bulkload

    // use tree->level to combine trees.
    int selectedLevel = 1;
    int numNodes = 0;
    list<KdbTree *> bulkingTrees;
    for (auto it = bkdTree->largeTrees.begin(); it != bkdTree->largeTrees.end();)
    {
        KdbTree *tmp = *it;
        if (tmp->level == selectedLevel)
        {
            bulkingTrees.push_back(tmp);
            numNodes += tmp->size;
        }
    }

    DataNode *values = new DataNode[numNodes * DIMENSIONS];

    int offset = 0;
    for (auto itr = bulkingTrees.begin(); itr != bulkingTrees.end(); ++itr)
    {
        KdbTree *treePtr = *itr;
        KdbTreeFetchNodes(treePtr, &values[offset]);
        offset += treePtr->size;
    }

    for (int d = 0; d < DIMENSIONS; d++)
    {
        if (d != 0) // paste over tree data
            memcpy(&values[d * numNodes], &values[0], sizeof(DataNode) * numNodes);

        std::sort(&values[d * numNodes], &values[d * numNodes + numNodes], dataNodeCMP(d));
    }

    KdbTree *tree = KdbCreateTree(values, numNodes, bkdTree->generateUniqueId(bkdTree->treeId), selectedLevel + 1);

    // TODO: add the tree to largetrees

    // remove old nodes
    delete[] values;
}

/*


    DataNode *values = new DataNode[numNodes * DIMENSIONS];

    // copy Mem and disk array
    memcpy(&values[0], localMemory, sizeof(DataNode) * localMemorySize);
    memcpy(&values[localMemorySize], localDisk, sizeof(DataNode) * localDiskSize);
    delete[] localMemory;
    delete[] localDisk;

    int offset = localMemorySize + localDiskSize;
    for (std::list<KdbTree *>::iterator itr = mergeTreeList.begin(); itr != mergeTreeList.end(); ++itr)
    {
        KdbTree *treePtr = *itr;
        KdbTreeFetchNodes(treePtr, &values[offset]);
        offset += treePtr->size;
    }

    for (int d = 0; d < DIMENSIONS; d++)
    {
        if (d != 0) // paste over tree data
            memcpy(&values[d * numNodes], &values[0], sizeof(DataNode) * numNodes);

        std::sort(&values[d * numNodes], &values[d * numNodes + numNodes], dataNodeCMP(d));
    }

    KdbTree *tree = KdbCreateTree(values, numNodes, generateUniqueId(treeId));

    if (treeArrayLocation != -1)
    { // store normal tree
        globalWriteTrees[treeArrayLocation] = tree;
    }
    else
    { // store large tree
        largeTrees.push_back(tree);
    }

    This section is responsible for:
    - RCU updating AtomicUnorderedMap
        ->  removing old trees from mapCopy(pointer and iterator)
    - Sending the removed values to scheduler so the scheduler can deal with it


// TODO: use writelock if multiple trees can update this section Create new map, rcu update
// IDEA: have a large trees scheduler, largetrees/later structures should never be blocked,
// but if it schedules the changes, bulkload could be responisble for inserting them
//  --> this is to avoid bulkloading getting locked..
AtomicUnorderedMapElement *mapCopy = new AtomicUnorderedMapElement;

if (globalReadMap == nullptr)
{
    mapCopy->readableTrees = new unordered_map<long, AtomicTreeElement *>();
}
else
{
    mapCopy->readableTrees = new unordered_map<long,
                                               AtomicTreeElement *>(*globalReadMap->readableTrees);
}

// for value in map: if value.id exists in mergeTreeList, delete it
for (auto it = mergeTreeList.begin(); it != mergeTreeList.end();)
{
    KdbTree *deleteTree = *it;

    mapCopy->readableTrees->erase(deleteTree->id);

    if (globalReadMap != NULL)
    {
        unordered_map<long, AtomicTreeElement *> *mapPtr = globalReadMap->readableTrees;

        auto itr = mapPtr->find(deleteTree->id);
        if (itr != mapPtr->end())
        {
            AtomicTreeElement *treeContainer = itr->second;
            treeContainer->deleted.store(true);
        }
        else
        {
            // OBS didnt find tree to delete??
            printf("ERROR DIDN'T FIND TREE TO DELETE SHOULDNT BE POSSIBLE\n");
            exit(-1);
        }
    }

    it = mergeTreeList.erase(it);
}
AtomicTreeElement *treeContainer = new AtomicTreeElement;
treeContainer->tree = tree;
treeContainer->treeId = tree->id;
mapCopy->readableTrees->insert(make_pair(tree->id, treeContainer));

AtomicUnorderedMapElement *oldMap = globalReadMap;
globalReadMap = mapCopy;

if (oldMap != nullptr)
{
    oldMap->deleted.store(true);
    while (true)
    {
        int index = -1;
        for (int i = 0; i < SCHEDULER_MAP_ARRAY_SIZE; i++)
        {
            AtomicUnorderedMapElement *ptr = schedulerDeletedMaps[i].load();
            if (ptr == nullptr)
            {
                index = i;
                break;
            }
        }
        AtomicUnorderedMapElement *expected = nullptr;
        if (index != -1 && schedulerDeletedMaps[index].compare_exchange_strong(expected, oldMap))
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
pthread_mutex_unlock(&bulkingLock);

// remove old nodes
delete[] values;
}
*/