#ifndef BKD_TREE_H
#define BKD_TREE_H
#include <list>
#include <unordered_map>
#include <unordered_set>
#include "Config.h"
#include "ThreadFunctions.h"

struct DataNode;
struct KdbTree;
class MockApi;
class BloomFilter;

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

class BkdTree
{
public:
    BkdTree();
    ~BkdTree();

    int insert(DataNode *values);
    void deleteNode(char *location);
    void _bulkloadTree();
    void updateReadTrees(list<KdbTree *> *mergeTreeList, KdbTree *tree);

    atomic<int> bulkLoadQueue = 0;
    MockApi *API = nullptr;
    DataNode *globalMemory = nullptr;
    atomic<int> globalMemorySize;

    DataNode *globalDisk = nullptr;
    atomic<int> globalDiskSize;

    bool globalChunkReady[GLOBAL_B_CHUNK_SIZE];

    pthread_mutex_t smallBulkingLock;
    KdbTree *globalWriteSmallTrees[MAX_BULKLOAD_LEVEL];

    pthread_mutex_t mediumWriteTreesLock;
    list<KdbTree *> globalWriteMediumTrees;

    atomic<int> largestLevel = 0;
    list<KdbTree *> globalWriteLargeTrees;

    atomic<long> treeId = 0;
    atomic<long> epoch = 0;

    // read values

    pthread_mutex_t globalReadMapWriteLock;
    atomic<AtomicUnorderedMapElement *> globalReadMap = nullptr;

    // TODO assert this never gets full..
    atomic<AtomicUnorderedMapElement *> schedulerDeletedMaps[SCHEDULER_MAP_ARRAY_SIZE];

    void deleteValue(char *location);
    bool isDeleted(char *location);
    bool deleteIfFound(char *location);

private:
    unordered_set<char *> tombstone;
    pthread_rwlock_t rwTombLock = PTHREAD_RWLOCK_INITIALIZER;
    // TODO/FILL: input age to avoid delete before inserted
    BloomFilter *graveFilter;
};

// have thread functions which just encapsulates BkdTree calls(?)

// need function that spawns thread that inserts data into memory buffer and returning it to global
// if global full, deal with global by pushing to disk/trees
// arg == Bkdtree?

void *_threadInserter(void *bkdTree);

#endif