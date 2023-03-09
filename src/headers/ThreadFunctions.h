#ifndef THREAD_STRUCTURES_H
#define THREAD_STRUCTURES_H
#include "MemoryStructures.h"
#include <list>
#include <atomic>
#include <unordered_map>
using namespace std;

class BkdTree;

struct WindowLookupInput
{
    BkdTree *tree;
    float window[DIMENSIONS][2];
    list<DataNode> results;
};

class KdbTree;

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

void *_threadInserterApi(void *writeThreadInput);

void *_windowLookup(void *input);
#endif