#include <iostream>
#include <list>
#include <string.h>
#include <algorithm>
#include <cmath>
#include "Config.h"
#include "BkdTree.h"
#include "KdbTree.h"

class dataNodeCMP
{
    int dimension;

public:
    dataNodeCMP(int dim) : dimension(dim) {}

    // This operator overloading enables calling
    // operator function () on objects of increment
    bool operator()(DataNode &a, DataNode &b)
    {
        return (bool)(a.cordinates[dimension] < b.cordinates[dimension]);
    }
};

inline long generateUniqueId(atomic<long> &counter)
{
    return counter.fetch_add(1);
}

BkdTree::BkdTree() // default constructor
{
    printf("Constructur runs!\n");
    API = new MockApi();
    globalMemory = new DataNode[GLOBAL_BUFFER_SIZE];
    globalMemorySize = 0;

    fill_n(globalChunkReady, GLOBAL_B_CHUNK_SIZE, false);
    globalDisk = NULL;
    globalDiskSize = 0;

    if (pthread_mutex_init(&bulkingLock, NULL) != 0)
    {
        printf("\n mutex init has failed\n");
        exit(0);
    }
}

BkdTree::~BkdTree() // Destructor
{
    printf("Destructor runs!\n");
    if (globalMemory != NULL)
    {
        printf("delete global!\n");
        delete[] globalMemory;
    }

    if (globalDisk != NULL)
    {
        delete[] globalDisk;
    }
    for (int i = 0; i < MAX_BULKLOAD_LEVEL; i++)
    {
        // TODO ask for globalWriteTrees lock?

        if (globalWriteTrees[i] != NULL)
        {
            KdbDestroyTree(globalWriteTrees[i]);
        }
    }

    delete API;
}

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
        pthread_exit(NULL);
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

    if (tree->globalDisk == NULL)
    {
        tree->globalDisk = tree->globalMemory;
        tree->globalDiskSize.store(GLOBAL_BUFFER_SIZE);

        fill_n(tree->globalChunkReady, GLOBAL_B_CHUNK_SIZE, false);

        tree->globalMemory = new DataNode[GLOBAL_BUFFER_SIZE];
        tree->globalMemorySize.store(0);
        printf("Used globalDisk\n");

        pthread_exit(NULL);
    }

    printf("Start bulkload\n");
    // clear tree->globalMemory and globalDisk
    // put old globalMemory and globalDisk into Read variable untill the data has been inserted (RCU)

    tree->_bulkloadTree();

    pthread_exit(NULL);
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
    globalDisk = NULL;
    globalDiskSize.store(0);
    globalMemorySize.store(0);

    int numNodes = localMemorySize + localDiskSize;

    pthread_mutex_lock(&bulkingLock);

    list<KdbTree *> mergeTreeList;
    int treeArrayLocation = 0;

    for (int currentTree = 0; currentTree < MAX_BULKLOAD_LEVEL; currentTree++)
    {
        int endTree = 0;

        if (globalWriteTrees[currentTree] != NULL)
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
            globalWriteTrees[previousTree] = NULL;

            if (treeArrayLocation != -1)
                treeArrayLocation = currentTree;

            break;
        }
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

    KdbTree *tree = KdbCreateTree(values, numNodes, generateUniqueId(treeId));

    if (treeArrayLocation != -1)
    { // store normal tree
        globalWriteTrees[treeArrayLocation] = tree;
    }
    else
    { // store large tree
        largeTrees.push_back(tree);
    }
    // TODO: use writelock if multiple trees can update this section Create new map, rcu update
    AtomicUnorderedMapElement *mapCopy = new AtomicUnorderedMapElement;
    mapCopy->readableTrees = new unordered_map<long, AtomicTreeElement *>(*globalReadMap->readableTrees);

    AtomicTreeElement *treeContainer = new AtomicTreeElement;
    treeContainer->tree = tree;
    treeContainer->treeId = tree->id;
    mapCopy->readableTrees->insert(make_pair(tree->id, treeContainer));

    AtomicUnorderedMapElement *oldMap = globalReadMap;
    globalReadMap = mapCopy;

    oldMap->deleted.store(true);
    if (oldMap->readers.load() <= 0)
    {
        delete oldMap->readableTrees;
        delete oldMap;
    }
    pthread_mutex_unlock(&bulkingLock);

    // remove old nodes
    delete[] values;

    for (std::list<KdbTree *>::iterator itr = mergeTreeList.begin(); itr != mergeTreeList.end(); ++itr)
    { // Fetch merged trees from readable trees and mark them as removed

        KdbTree *tmpTree = *itr;
        unordered_map<long, AtomicTreeElement *>::iterator it = readableTrees.find(tmpTree->id);
        if (it != readableTrees.end())
        {
            AtomicTreeElement *value = it->second;
            value->deleted.store(true);
            if (value->readers.load() <= 0)
            {
                KdbDestroyTree(value->tree);
                value->tree = NULL;
            }
        }
        // TODO: does deleting the tree crash the iterator(?)

        mergeTreeList.erase(itr++);
    }
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
            -> reader++, sjekk if slettet, hvis slettet og reader == 1 sjekk at tree == NULL før dra videre
        - tree ikke slettet: les data
            -> ferdiglest:
                - reader--
                - sjekk at treet ikke er slettet
                    -> hvis slettet sjekk at reader != 0
                        -> hvis == 0, sjekk at tree == NULL
                            -> hvis ikke, slett treet



*/
