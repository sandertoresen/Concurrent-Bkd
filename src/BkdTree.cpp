#include <iostream>
#include <list>
#include <set>
#include <string.h>
#include <algorithm>
#include "headers/Config.h"
#include "headers/BkdTree.h"
#include "headers/KdbTree.h"
#include "headers/MockAPI.h"
#include "headers/ThreadFunctions.h"
#include "headers/BloomFilter.h"

BkdTree::BkdTree() // default constructor
{
    printf("Constructur runs!\n");
    API = new MockApi();
    globalMemory = new DataNode[GLOBAL_BUFFER_SIZE];
    globalMemorySize = 0;

    fill_n(globalChunkReady, GLOBAL_B_CHUNK_SIZE, false);
    fill_n(globalWriteSmallTrees, MAX_BULKLOAD_LEVEL, nullptr);
    fill_n(schedulerDeletedMaps, SCHEDULER_MAP_ARRAY_SIZE, nullptr);

    globalDisk = nullptr;
    globalDiskSize = 0;

    if (pthread_mutex_init(&smallBulkingLock, nullptr) != 0)
    {
        printf("\n mutex init has failed\n");
        exit(0);
    }
    if (pthread_mutex_init(&globalReadMapWriteLock, nullptr) != 0)
    {
        printf("\n mutex init has failed\n");
        exit(0);
    }
    if (pthread_mutex_init(&mediumWriteTreesLock, nullptr) != 0)
    {
        printf("\n mutex init has failed\n");
        exit(0);
    }

    graveFilter = new BloomFilter(BLOOM_SIZE, BLOOM_NUM_HASHES);
}

BkdTree::~BkdTree() // Destructor
{
    printf("Destructor runs!\n");

    delete API;

    if (globalMemory != nullptr)
    {
        printf("delete global!\n");
        delete[] globalMemory;
    }

    if (globalDisk != nullptr)
    {
        delete[] globalDisk;
    }

    pthread_mutex_destroy(&smallBulkingLock);

    set<KdbTree *> deletedTrees;
    // set<KdbTree *> deletedTreeContainers;
    for (int i = 0; i < MAX_BULKLOAD_LEVEL; i++)
    {
        if (globalWriteSmallTrees[i] != nullptr)
        {
            deletedTrees.insert(globalWriteSmallTrees[i]);
        }
    }

    pthread_mutex_destroy(&mediumWriteTreesLock);
    for (auto kdbTree : globalWriteMediumTrees)
    {
        deletedTrees.insert(kdbTree);
    }
    globalWriteMediumTrees.clear();

    for (auto kdbTree : globalWriteLargeTrees)
    {
        deletedTrees.insert(kdbTree);
    }
    globalWriteLargeTrees.clear();

    pthread_mutex_destroy(&globalReadMapWriteLock);
    if (globalReadMap != nullptr)
    {
        for (auto it = globalReadMap->readableTrees->begin(); it != globalReadMap->readableTrees->end();)
        {
            // TODO OBS we don't check for active readers, make this safe with scheduler
            KdbTree *tmp = it->second;
            deletedTrees.insert(tmp);
            // deletedTreeContainers.insert(tmp);
            it = globalReadMap->readableTrees->erase(it);
        }
        delete globalReadMap->readableTrees;
        delete globalReadMap;
    }

    // atomic<AtomicUnorderedMapElement *> schedulerDeletedMaps[SCHEDULER_MAP_ARRAY_SIZE];
    for (int i = 0; i < SCHEDULER_MAP_ARRAY_SIZE; i++)
    {
        AtomicUnorderedMapElement *deletedElement = schedulerDeletedMaps[i];
        if (deletedElement == nullptr)
            continue;
        for (auto it = deletedElement->readableTrees->begin(); it != deletedElement->readableTrees->end();)
        {
            KdbTree *tmp = it->second;
            deletedTrees.insert(tmp);

            // deletedTreeContainers.insert(tmp);
            it = deletedElement->readableTrees->erase(it);
        }
        delete deletedElement->readableTrees;
        delete deletedElement;
    }

    for (auto kdbTree : deletedTrees)
    {
        KdbDestroyTree(kdbTree);
    }
    deletedTrees.clear();

    /*for (auto container : deletedTreeContainers)
    {
        delete container;
    }
    deletedTreeContainers.clear();*/

    for (auto location : tombstone)
    {
        delete location;
    }

    pthread_rwlock_destroy(&rwTombLock);
    delete graveFilter;
}

// pointers to take in Memory array, Disk array
void BkdTree::_bulkloadTree()
{
    /* Function assumption: we only need to do one bulkload at a time */
    DataNode *localMemory = globalMemory;
    DataNode *localDisk = globalDisk;

    int localMemorySize = globalMemorySize.load();
    int localDiskSize = globalDiskSize.load();

    globalMemory = new DataNode[GLOBAL_BUFFER_SIZE];
    fill_n(globalChunkReady, GLOBAL_B_CHUNK_SIZE, false);
    globalDisk = nullptr;
    globalMemorySize.store(0);
    globalDiskSize.store(0);

    int numNodes = localMemorySize + localDiskSize;

    pthread_mutex_lock(&smallBulkingLock);

    list<KdbTree *> mergeTreeList;
    int treeArrayLocation = 0;

    for (int currentTree = 0; currentTree < MAX_BULKLOAD_LEVEL; currentTree++)
    {
        int endTree = 0;

        if (globalWriteSmallTrees[currentTree] != nullptr)
        {
            if (currentTree + 1 != MAX_BULKLOAD_LEVEL)
                continue;
            else
            {
                treeArrayLocation = -1;
                endTree = MAX_BULKLOAD_LEVEL;
            }
        }
        if (endTree == 0)
            endTree = currentTree;

        for (int previousTree = 0; previousTree < endTree; previousTree++)
        {
            mergeTreeList.push_back(globalWriteSmallTrees[previousTree]);
            numNodes += globalWriteSmallTrees[previousTree]->size;
            globalWriteSmallTrees[previousTree] = nullptr;

            if (treeArrayLocation != -1)
                treeArrayLocation = currentTree;
        }
        break;
    }

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

    KdbTree *tree = KdbCreateTree(values, numNodes, generateUniqueId(treeId), 0);
    delete[] values;
    if (treeArrayLocation != -1)
    { // store normal tree
        globalWriteSmallTrees[treeArrayLocation] = tree;
    }
    else
    { // store large tree
        pthread_mutex_lock(&mediumWriteTreesLock);
        globalWriteMediumTrees.push_back(tree);
        pthread_mutex_unlock(&mediumWriteTreesLock);
    }

    /*
    This section is responsible for:
    - RCU updating AtomicUnorderedMap
        ->  removing old trees from mapCopy(pointer and iterator)
    - Sending the removed values to scheduler so the scheduler can deal with it
    */

    // TODO: use writelock if multiple trees can update this section Create new map, rcu update
    // IDEA: have a large trees scheduler, largetrees/later structures should never be blocked,
    // but if it schedules the changes, bulkload could be responisble for inserting them
    //  --> this is to avoid bulkloading getting locked..

    AtomicUnorderedMapElement *mapCopy = new AtomicUnorderedMapElement;

    pthread_mutex_lock(&globalReadMapWriteLock);
    if (globalReadMap == nullptr)
    {
        mapCopy->readableTrees = new unordered_map<long, KdbTree *>();
    }
    else
    {
        mapCopy->readableTrees = new unordered_map<long,
                                                   KdbTree *>(*globalReadMap->readableTrees);
    }

    // for value in map: if value.id exists in mergeTreeList, delete it
    for (auto it = mergeTreeList.begin(); it != mergeTreeList.end();)
    {
        KdbTree *deleteTree = *it;

        mapCopy->readableTrees->erase(deleteTree->id);

        if (globalReadMap != NULL)
        {
            unordered_map<long, KdbTree *> *mapPtr = globalReadMap->readableTrees;

            auto itr = mapPtr->find(deleteTree->id);
            if (itr != mapPtr->end())
            {
                KdbTree *treeContainer = itr->second;
                printf("Delete treeId: %d\n", deleteTree->id);
                treeContainer->deleted.store(true);
            }
            else
            {
                // OBS didnt find tree to delete??
                printf("|small bulkload|ERROR DIDN'T FIND TREE TO DELETE SHOULDNT BE POSSIBLE\n");
                exit(-1);
            }
        }

        it = mergeTreeList.erase(it);
    }
    // AtomicTreeElement *treeContainer = new AtomicTreeElement;
    // treeContainer->tree = tree;
    mapCopy->readableTrees->insert(make_pair(tree->id, tree));

    AtomicUnorderedMapElement *oldMap = globalReadMap;
    globalReadMap = mapCopy;
    pthread_mutex_unlock(&globalReadMapWriteLock);

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
    pthread_mutex_unlock(&smallBulkingLock);
}

void BkdTree::deleteValue(char *location)
{
    pthread_rwlock_wrlock(&rwTombLock);
    tombstone.insert(location);
    pthread_rwlock_unlock(&rwTombLock);
    graveFilter->add(location);
}
bool BkdTree::isDeleted(char *location)
{
    if (graveFilter->contains(location))
    {
        pthread_rwlock_rdlock(&rwTombLock);
        if (tombstone.find(location) != tombstone.end())
        {
            pthread_rwlock_unlock(&rwTombLock);
            return true;
        }
        pthread_rwlock_unlock(&rwTombLock);
    }
    return false;
}
bool BkdTree::deleteIfFound(char *location)
{
    if (graveFilter->contains(location))
    {
        pthread_rwlock_rdlock(&rwTombLock);
        auto iter = tombstone.find(location);
        if (iter != tombstone.end())
        {
            pthread_rwlock_unlock(&rwTombLock);
            pthread_rwlock_wrlock(&rwTombLock);
            delete[] (*iter);
            tombstone.erase(iter);
            pthread_rwlock_unlock(&rwTombLock);
            return true;
        }
        pthread_rwlock_unlock(&rwTombLock);
    }
    return false;
}
