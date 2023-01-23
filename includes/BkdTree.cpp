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

    int size;

    while (1)
    {
        size = tree->globalMemorySize.load();
        int updatedSize = size + THREAD_BUFFER_SIZE;

        if (size >= GLOBAL_BUFFER_SIZE)
            continue;
        if (tree->globalMemorySize.compare_exchange_weak(size, updatedSize))
            break;
    }

    memcpy(&tree->globalMemory[size],
           &threadData, sizeof(DataNode) * THREAD_BUFFER_SIZE);

    printf("Size: %d Set value at chunk %d\n", size, size / THREAD_BUFFER_SIZE);
    tree->globalChunkReady[size / THREAD_BUFFER_SIZE] = true;
    // printf("fetch %s\n", tree->globalMemory->threadDataNodeBuffers[size]->buffer->location);
    /*
//TODO: flag?
GlobalMem[*,*,*] //pointers
Finished inserting[-1,-1,-1]
first size ++;
then insert data
update finished inserting

finished inserting will be used by thread to assert data
is safe to handle and other nodes are finished



load size
if(size != full)
compare exchange(replace with size + 1)
insert data


while(buf not full):
    wait for Api input
    buf[i] = stream[i++];

//buf full
while(GlobalMem full)
    wait

try inserting data

if Globalmem now full, handle restructure
*/
    pthread_exit(NULL);
}
