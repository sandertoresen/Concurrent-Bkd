#ifndef BKD_TREE_H
#define BKD_TREE_H
#include <list>
#include <unordered_map>
#include "Config.h"
#include "ThreadFunctions.h"

struct DataNode;
struct KdbTree;
class MockApi;

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
inline long generateUniqueId(atomic<long> &counter);

class BkdTree
{
public:
    BkdTree();
    ~BkdTree();

    int insert(DataNode *values);
    void windowLookup(list<DataNode> &values, int window[DIMENSIONS][2]);
    void _bulkloadTree();

    MockApi *API;
    DataNode *globalMemory;
    atomic<int> globalMemorySize;

    DataNode *globalDisk;
    atomic<int> globalDiskSize;

    bool globalChunkReady[GLOBAL_B_CHUNK_SIZE];

    // must assert no other readers have started bulkloading in the meantime, might require lock

    pthread_mutex_t smallBulkingLock;

    KdbTree *globalWriteSmallTrees[MAX_BULKLOAD_LEVEL];

    pthread_mutex_t mediumWriteTreesLock;
    list<KdbTree *> globalWriteMediumTrees;

    atomic<int> largestLevel = 0;
    list<KdbTree *> globalWriteLargeTrees;

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