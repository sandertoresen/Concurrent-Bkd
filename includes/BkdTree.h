#ifndef BKD_TREE_H
#define BKD_TREE_H
#include <list>
#include <unordered_map>
#include <cstdarg>
#include "Config.h"
#include "MemoryStructures.h"
#include "KdbTree.h"
#include "MockAPI.h"

class BkdTree
{
public:
    BkdTree();
    ~BkdTree();

    // int insert(DataNode *value);
    int insert(DataNode *values);
    void windowLookup(list<DataNode> &values, int window[DIMENSIONS][2]);
    // int remove(float index[DIMENSIONS]);
    MockApi *API;
    DataNode *globalMemory;
    atomic<int> globalMemorySize;

    DataNode *globalDisk;
    atomic<int> globalDiskSize;

    bool globalChunkReady[GLOBAL_B_CHUNK_SIZE];

    // must assert no other readers have started bulkloading in the meantime, might require lock

    pthread_mutex_t bulkingLock;
    atomic<int> treeStorageStatus[MAX_BULKLOAD_LEVEL]; // 0 -> free, 1 -> stored tree
    KdbTree *globalWriteTrees[MAX_BULKLOAD_LEVEL];

    list<KdbTree *> largeTrees;
    // list<KdbTree> *globalReadTree; //list pointer for readers (RCU)

    void _bulkloadTree();

    atomic<long> treeId = 0;
    atomic<long> arrayId = 0;

    // read values
    unordered_map<int, AtomicTreeElement *> readableTrees;
};

// have thread functions which just encapsulates BkdTree calls(?)

// need function that spawns thread that inserts data into memory buffer and returning it to global
// if global full, deal with global by pushing to disk/trees
// arg == Bkdtree?

void *_threadInserter(void *bkdTree);

#endif