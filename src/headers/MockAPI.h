#ifndef MOCK_API_THREAD_H
#define MOCK_API_THREAD_H
#include <atomic>
#include <list>
#include "Config.h"
#include "MemoryStructures.h"
#include "ThreadFunctions.h"

using namespace std;

class Scheduler;

class MockApi
{
public:
    MockApi();
    MockApi(char *FILE);
    MockApi(int mockSize);
    ~MockApi();
    atomic<int> delay;
    float min = -1, max = -1;
    // current threads
    // timeout range

    Scheduler *scheduler;

    float (*selectedRandomFunc)(float, float) = nullptr;

    DataNode *fetchRandom(DataNode *node);
    WindowQuery *fetchWindowQuery();
};

#endif