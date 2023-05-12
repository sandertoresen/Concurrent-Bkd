#ifndef THREAD_STRUCTURES_H
#define THREAD_STRUCTURES_H
#include <atomic>
#include <list>
#include <pthread.h>
#include <unordered_map>
#include "MemoryStructures.h"

using namespace std;

class BkdTree;
class KdbTree;
class Scheduler;

struct WindowQuery
{
    float window[DIMENSIONS][2];
    list<DataNode> results;
};

struct ScheduledThread
{
    pthread_t thread;
    int threadId;
    int numTrees;
    BkdTree *tree;
    atomic<int> flag;
    atomic<long> epoch = 0;
};

struct BulkLoadThread
{
    pthread_t thread;
    Scheduler *scheduler;
    atomic<int> flag;
};

/*struct AtomicTreeElement
{
    // readers flag
    // atomic<int> readers = 0;
    atomic<bool> deleted = false;
    KdbTree *tree;
};*/

struct AtomicUnorderedMapElement
{
    atomic<long> epoch = 0;
    list<KdbTree *> *oldTrees = nullptr;
    unordered_map<long, KdbTree *> *readableTrees;
};

void *_threadInserter(void *writerThread);

void *_windowLookup(void *input);

void *_performLargerBulkLoad(void *bulkLoadThread);
#endif