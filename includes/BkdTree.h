#ifndef BKD_TREE_H
#define BKD_TREE_H
#include <list>
#include <cstdarg>
#include "Config.h"
#include "MemoryStructures.h"
#include "KdbTree.h"

class BkdTree
{
public:
    BkdTree();
    ~BkdTree();

    // int insert(DataNode *value);
    int insert(DataNode *values);
    // int remove(float index[DIMENSIONS]);

    DataNode *globalMemory;
    atomic<int> globalMemorySize;

    DataNode *globalDisk;
    atomic<int> globalDiskSize;

    bool globalChunkReady[GLOBAL_B_CHUNK_SIZE];

    // must assert no other readers have started bulkloading in the meantime, might require lock

    pthread_mutex_t bulkingLock;
    atomic<int> treeBulkingStatus[MAX_BULKLOAD_LEVEL]; // 0 -> free, 1 -> stored tree, -1 -> currently bulkloading
    KdbTree *globalWriteTreesArr[MAX_BULKLOAD_LEVEL];

    // TODO: remove this variable
    list<KdbTree *> globalWriteTree;

    // structures currently beeing processed can be temporarily accessed here
    // list<void *> readSafeStructures;

    // list<KdbTree> *globalReadTree; //list pointer for readers (RCU)

    void _bulkloadTree();

    // list<KDB_Tree> firstTreeStorage // same growth sequence as prepatory project

    // list<KDB_Tree> trees of a sertain size
    // num trees of sertain size
};

// have thread functions which just encapsulates BkdTree calls(?)

// need function that spawns thread that inserts data into memory buffer and returning it to global
// if global full, deal with global by pushing to disk/trees
// arg == Bkdtree?

void *_threadInserter(void *bkdTree);

#endif