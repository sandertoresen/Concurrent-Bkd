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
    globalMemory = new DataNode[GLOBAL_BUFFER_SIZE];
    globalMemorySize = 0;

    fill_n(globalChunkReady, GLOBAL_B_CHUNK_SIZE, false);
    globalDisk = NULL;
    globalDiskSize = -1;
}

BkdTree::~BkdTree() // Destructor
{
    printf("Destructor runs!\n");
    if (globalMemory != NULL)
    {
        delete[] globalMemory;
    }

    if (globalDisk != NULL)
    {
        delete[] globalDisk;
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
        threadData[i].cordinates[0] = (float)i;
        threadData[i].cordinates[1] = (float)i;
        strcpy(threadData[i].location, "Input");
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
        return NULL;
        // pthread_exit(NULL);
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
        return NULL;
        // pthread_exit(NULL);
    }

    printf("Start bulkload\n");
    tree->_bulkloadTree();
    return NULL;
    // pthread_exit(NULL);
}

// pointers to take in Memory array, Disk array
void BkdTree::_bulkloadTree()
{
    // Step 1: copy over Datanodes and reset global Memory and disk to get other threads working again!
    // update safe reader pointer so data can still be accessed.
    // OBS assert data is safe to edit

    // atomic variables(?)
    int localMemoryBufferSize = globalMemorySize.load();
    int localDiskBufferSize = globalDiskSize.load();
    int numNodes = localMemoryBufferSize + localDiskBufferSize;
    int treeSize = 0;

    // will add tree last if not changed
    bool addLast = true;
    std::list<KdbTree *>::iterator newTreelocation;

    if (globalWriteTree.size())
    {
        for (std::list<KdbTree *>::iterator itr = globalWriteTree.begin(); itr != globalWriteTree.end(); ++itr)
        {
            KdbTree *ptr = *itr;
            if (ptr == NULL)
            {
                addLast = false;
                newTreelocation = itr;
                break;
            }
            // TODO update tree size when deleting
            treeSize += ptr->size;
        }
    }

    numNodes += treeSize;

    // allocate temporary file F
    DataNode *values = new DataNode[numNodes * DIMENSIONS];

    int offset = 0;
    for (std::list<KdbTree *>::iterator itr = globalWriteTree.begin(); itr != globalWriteTree.end(); ++itr)
    { // copy all values from globalWriteTree
        printf("|x| In for loop\n...\n");
        // break if we reach replacement node
        if (!addLast && itr == newTreelocation)
            break;

        KdbTree *ptr = *itr;
        // insert data on first dimention |Mem|Disk|*Tree*
        KdbTreeFetchNodes(ptr, &values[localMemoryBufferSize + localDiskBufferSize + offset]);
        offset += ptr->size;
    }

    // copy Mem and disk array
    memcpy(&values[0], globalMemory, sizeof(DataNode) * localMemoryBufferSize);
    memcpy(&values[localMemoryBufferSize], globalDisk, sizeof(DataNode) * localDiskBufferSize);

    // memcpy mem and disk arrays
    for (int d = 0; d < DIMENSIONS; d++)
    {
        if (d) // paste over tree data
            memcpy(&values[d * numNodes], &values[0], sizeof(DataNode) * numNodes);

        std::sort(&values[d * numNodes], &values[d * numNodes + numNodes], dataNodeCMP(d));
    }
    /*for (int i = 0; i < numNodes; i++)
    {
        printf("|%d|(%f,%f):%s\n", i,
               values[i].cordinates[0],
               values[i].cordinates[1],
               values[i].location);
    }*/

    printf("|x| Create KdbTree\n");
    KdbTree *tree = KdbCreateTree(values, numNodes);

    // add tree at correct position
    if (addLast)
    {
        for (std::list<KdbTree *>::iterator itr = globalWriteTree.begin(); itr != globalWriteTree.end(); ++itr)
        {
            KdbTree *ptr = *itr;
            // KdbTree_destroy(ptr);
            itr = globalWriteTree.erase(itr);
            itr = globalWriteTree.insert(itr, NULL);
        }

        globalWriteTree.push_back(tree);
    }
    else
    {
        // remove all nodes before it and replace it
        newTreelocation = globalWriteTree.erase(newTreelocation);
        newTreelocation = globalWriteTree.insert(newTreelocation, tree);

        // delete all nodes before
        for (std::list<KdbTree *>::iterator itr = globalWriteTree.begin(); itr != newTreelocation; ++itr)
        {
            KdbTree *ptr = *itr;
            // KdbTree_destroy(ptr);
            itr = globalWriteTree.erase(itr);
            itr = globalWriteTree.insert(itr, NULL);
        }
    }

    // remove old nodes
    // delete[] values;
    // delete MemArray;
    // delete DiskArray;

    // MemArray = new DataArray;
    localMemoryBufferSize = 0;

    // DiskArray = NULL;
    // DiskArrayFull = false;
    // __printKdbTree(tree);
}
