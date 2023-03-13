#include <iostream>
#include <list>
#include <string.h>
#include <algorithm>
#include "headers/Config.h"
#include "headers/BkdTree.h"
#include "headers/KdbTree.h"
#include "headers/MockAPI.h"
#include "headers/ThreadFunctions.h"

BkdTree::BkdTree() // default constructor
{
    printf("Constructur runs!\n");
    API = new MockApi();
    globalMemory = new DataNode[GLOBAL_BUFFER_SIZE];
    globalMemorySize = 0;

    fill_n(globalChunkReady, GLOBAL_B_CHUNK_SIZE, false);
    fill_n(globalWriteTrees, MAX_BULKLOAD_LEVEL, nullptr);
    fill_n(schedulerDeletedMaps, SCHEDULER_MAP_ARRAY_SIZE, nullptr);

    globalDisk = nullptr;
    globalDiskSize = 0;

    if (pthread_mutex_init(&bulkingLock, nullptr) != 0)
    {
        printf("\n mutex init has failed\n");
        exit(0);
    }
}

BkdTree::~BkdTree() // Destructor
{
    printf("Destructor runs!\n");
    if (globalMemory != nullptr)
    {
        printf("delete global!\n");
        delete[] globalMemory;
    }

    if (globalDisk != nullptr)
    {
        delete[] globalDisk;
    }

    if (globalReadMap != nullptr)
    {
        for (auto it = globalReadMap->readableTrees->begin(); it != globalReadMap->readableTrees->end();)
        {
            // TODO OBS we don't check for active readers, make this safe with scheduler
            AtomicTreeElement *tmp = it->second;
            KdbDestroyTree(tmp->tree);
            delete tmp;
            it = globalReadMap->readableTrees->erase(it);
        }
        delete globalReadMap->readableTrees;
        delete globalReadMap;
    }
    delete API;
}

inline long BkdTree::generateUniqueId(atomic<long> &counter)
{
    return counter.fetch_add(1);
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
    globalDiskSize.store(0);
    globalMemorySize.store(0);

    int numNodes = localMemorySize + localDiskSize;

    pthread_mutex_lock(&bulkingLock);

    list<KdbTree *> mergeTreeList;
    int treeArrayLocation = 0;

    for (int currentTree = 0; currentTree < MAX_BULKLOAD_LEVEL; currentTree++)
    {
        int endTree = 0;

        if (globalWriteTrees[currentTree] != nullptr)
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
            mergeTreeList.push_back(globalWriteTrees[previousTree]);
            numNodes += globalWriteTrees[previousTree]->size;
            globalWriteTrees[previousTree] = nullptr;

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

    KdbTree *tree = KdbCreateTree(values, numNodes, generateUniqueId(treeId), -1);

    if (treeArrayLocation != -1)
    { // store normal tree
        globalWriteTrees[treeArrayLocation] = tree;
    }
    else
    { // store large tree
        tree->level = 1;
        largeTrees.push_back(tree);
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

/*
    Problemstilling:
    Problem:
        trenger concurrent list med trær som kan RCU
    MVP:
        liste med struct som inneholder atomiske flagg
        start: ingen cleanup
        start: ikke lagre arrays (FILL)


    ADDITIONS:
        locked(list cleanup)

    LATER:
        Readers free data if int readers < 0

    1. struct
    2. list

    IGJENN readers oppgave:
     1. hente ut en lokal versjon av GlobalMap
        - iterer over AtomicTreeElement assert element ikke er slettet
            -> reader++, sjekk if slettet, hvis slettet og reader == 1 sjekk at tree == nullptr før dra videre
        - tree ikke slettet: les data
            -> ferdiglest:
                - reader--
                - sjekk at treet ikke er slettet
                    -> hvis slettet sjekk at reader != 0
                        -> hvis == 0, sjekk at tree == nullptr
                            -> hvis ikke, slett treet



*/
