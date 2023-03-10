#ifndef BKD_TREE_H
#define BKD_TREE_H
#include <list>
#include <unordered_map>
#include "Config.h"
#include "ThreadFunctions.h"

struct DataNode;
struct KdbTree;
class MockApi;

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

    KdbTree *globalWriteTrees[MAX_BULKLOAD_LEVEL];

    list<KdbTree *> largeTrees;
    // list<KdbTree> *globalReadTree; //list pointer for readers (RCU)

    void _bulkloadTree();

    atomic<long> treeId = 0;

    // read values

    AtomicUnorderedMapElement *globalReadMap = nullptr;
    pthread_mutex_t globalReadMapWriteLock;

    // TODO assert this never gets full..
    atomic<AtomicUnorderedMapElement *> schedulerDeletedMaps[SCHEDULER_MAP_ARRAY_SIZE];
};

// have thread functions which just encapsulates BkdTree calls(?)

// need function that spawns thread that inserts data into memory buffer and returning it to global
// if global full, deal with global by pushing to disk/trees
// arg == Bkdtree?

void *_threadInserter(void *bkdTree);

#endif