#include <iostream>
#include <list>
#include <string.h>
#include <algorithm>
#include <cmath>
#include "Config.h"
#include "BkdTree.h"

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
        delete globalMemory;
    }

    if (globalDisk != NULL)
    {
        delete globalDisk;
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
        threadData[i].cordinates[0] = 20.5f;
        threadData[i].cordinates[1] = 63.5f;
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

    printf("Obs.. not implemented yet, bulkloading\n");
    pthread_exit(NULL);
}

// pointers to take in Memory array, Disk array
void BkdTree::_bulkloadTree()
{
    /*// step 1, find first aveilable tree

    // count how many trees and nodes need inserting
    int num_nodes = MemArray->size + DiskArray->size;
    int trees_size = 0;

    // will add tree last if not changed
    bool add_last = true;
    std::list<KDB_Tree *>::iterator it;

    if (trees.size())
    {
        for (std::list<KDB_Tree *>::iterator itr = trees.begin(); itr != trees.end(); ++itr)
        {
            KDB_Tree *ptr = *itr;
            if (ptr == NULL)
            {
                add_last = false;
                it = itr; // store pointer tree will be inserted
                break;
            }
            // TODO update tree size when deleting
            trees_size += ptr->size;
        }
    }

    num_nodes += trees_size;

    // allocate temporary file F, (store data DIMENSIONS times to be able to sort it on every dimention)

    DataNode *values = new DataNode[num_nodes * DIMENSIONS]; // allocate space for nodes per dim
    int offset = 0;
    for (std::list<KDB_Tree *>::iterator itr = trees.begin(); itr != trees.end(); ++itr)
    { // copy all values from trees
        // break if we reach replacement node
        if (!add_last && itr == it)
            break;

        KDB_Tree *ptr = *itr;
        // insert data on first dimention |Mem|Disk|*Tree*
        KDB_Tree_fetch_all_nodes(ptr, &values[MemArray->size + DiskArray->size + offset]);
        offset += ptr->size;
    }

    // copy Mem and disk array
    memcpy(&values[0], MemArray->array, sizeof(DataNode) * MemArray->size);
    memcpy(&values[MemArray->size], DiskArray->array, sizeof(DataNode) * DiskArray->size);

    // memcpy mem and disk arrays
    for (int d = 0; d < DIMENSIONS; d++)
    {
        if (d) // paste over tree data
            memcpy(&values[d * num_nodes], &values[0], sizeof(DataNode) * num_nodes);

        std::sort(&values[d * num_nodes], &values[d * num_nodes + num_nodes], dataNodeCMP(d));
    }

    KDB_Tree *tree = Skratch_KDB_Create_Tree(values, num_nodes);

    // add tree at correct position
    if (add_last)
    {
        for (std::list<KDB_Tree *>::iterator itr = trees.begin(); itr != trees.end(); ++itr)
        {
            KDB_Tree *ptr = *itr;
            // KDB_Tree_destroy(ptr);
            itr = trees.erase(itr);
            itr = trees.insert(itr, NULL);
        }

        trees.push_back(tree);
    }
    else
    {
        // remove all nodes before it and replace it
        it = trees.erase(it);
        it = trees.insert(it, tree);

        // delete all nodes before
        for (std::list<KDB_Tree *>::iterator itr = trees.begin(); itr != it; ++itr)
        {
            KDB_Tree *ptr = *itr;
            // KDB_Tree_destroy(ptr);
            itr = trees.erase(itr);
            itr = trees.insert(itr, NULL);
        }
    }

    // remove old nodes
    delete[] values;
    delete MemArray;
    delete DiskArray;

    MemArray = new DataArray;
    MemArray->size = 0;

    DiskArray = NULL;
    DiskArrayFull = false;*/
}
