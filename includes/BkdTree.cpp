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

BkdTree::BkdTree() // default constructor
{
    printf("Constructur runs!\n");
    API = new MockApi();
    globalMemory = new DataNode[GLOBAL_BUFFER_SIZE];
    globalMemorySize = 0;

    fill_n(globalChunkReady, GLOBAL_B_CHUNK_SIZE, false);
    fill_n(treeBulkingStatus, MAX_BULKLOAD_LEVEL, 0);
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
        if (treeBulkingStatus[i] == -1)
        {
            printf("Error thread still working on tree after deconstruction!\n");
            exit(-1);
        }
        if (treeBulkingStatus[i] == 1)
        {
            KdbDestroyTree(globalWriteTrees[i]);
        }
    }
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
    // Clear globalDisk and globalMem
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

    /*
    Fix everything needed with syncronization
    TODOS:
    - Define work area and which trees will be merged
    - Remove trees we're working on from writeTree list
    - Add trees we're working on to ReadTree list
    - update bulkingArray(?)
    */
    list<KdbTree *> mergedTreeList;
    int treeArrayLocation = -1;

    // while (treeArrayLocation == -1) //TODO: spin untill slot opens up
    // if all are full, merge them all together to large tree

    for (int currentTree = 0; currentTree < MAX_BULKLOAD_LEVEL; currentTree++)
    {
        if (treeBulkingStatus[currentTree].load() != 0)
            continue;

        for (int previousTree = 0; previousTree < currentTree; previousTree++)
        {
            if (treeBulkingStatus[previousTree] == 1)
            {
                mergedTreeList.push_back(globalWriteTrees[previousTree]);
                numNodes += globalWriteTrees[previousTree]->size;
                globalWriteTrees[previousTree] = NULL;
                treeBulkingStatus[previousTree] = 0;
            }
            else
            {
                // TODO: get thread to cancel bulk load and return nodes
                // -> expect to recive DataNode array with size
                // -> how to communicate and share data between threads(?)
                //      -> instead of -1, could use flag to communicate where to "post messages to eachother"!
                //      -> create messaging system with struct{DataNode *arr, int arrSize, bool cancel}
                //      -> list/array of structs(?)
                printf("OBS!.. bulkLoading conflict not yet handled....\n");
                exit(0);
            }
        }
        treeBulkingStatus[currentTree] = -1; // set to working on
        treeArrayLocation = currentTree;
        break;
    }

    pthread_mutex_unlock(&bulkingLock);

    DataNode *values = new DataNode[numNodes * DIMENSIONS];

    // copy Mem and disk array
    memcpy(&values[0], localMemory, sizeof(DataNode) * localMemorySize);
    memcpy(&values[localMemorySize], localDisk, sizeof(DataNode) * localDiskSize);
    delete[] localMemory;
    delete[] localDisk;

    int offset = localMemorySize + localDiskSize;
    for (std::list<KdbTree *>::iterator itr = mergedTreeList.begin(); itr != mergedTreeList.end(); ++itr)
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
    for (int i = 0; i < numNodes; i++)
    {
        printf("|%d|(%f,%f):%s\n", i,
               values[i].cordinates[0],
               values[i].cordinates[1],
               values[i].location);
    }

    KdbTree *tree = KdbCreateTree(values, numNodes);
    globalWriteTrees[treeArrayLocation] = tree;
    treeBulkingStatus[treeArrayLocation].store(1);
    // TODO update Read log and remove merged trees from it

    // remove old nodes
    delete[] values;
    // delete list<KdbTree *> mergedTreeList;

    for (std::list<KdbTree *>::iterator itr = mergedTreeList.begin(); itr != mergedTreeList.end(); ++itr)
    {
        KdbDestroyTree(*itr);
        mergedTreeList.erase(itr++);
    }

    // __printKdbTree(tree);
}
