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

struct WindowQuery
{
    float window[DIMENSIONS][2];
    list<DataNode> results;
};

struct ScheduledThread
{
    pthread_t thread;
    BkdTree *tree;
    atomic<int> flag;
};

struct AtomicTreeElement
{
    // readers flag
    long treeId; // id for writers to locate variable
    // atomic<int> readers = 0;
    atomic<bool> deleted = false;
    KdbTree *tree;
};

struct AtomicUnorderedMapElement
{
    atomic<int> readers = 0;
    atomic<bool> deleted = false;
    unordered_map<long, AtomicTreeElement *> *readableTrees;
};

void *_threadInserter(void *bkdTree);

void *_threadInserterControlled(void *writerThread);

void *_windowLookup(void *input);
#endif