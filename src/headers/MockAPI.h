#ifndef MOCK_API_THREAD_H
#define MOCK_API_THREAD_H
#include <atomic>
#include <list>
#include "Config.h"
#include "MemoryStructures.h"

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
    // current threads
    // timeout range

    Scheduler *scheduler;
    DataNode *mockData = nullptr;
    atomic<int> mockDataPtr = 0;

    int generateMockData();
    int loadMockData(char *FILE);

    DataNode *fetchRandom(DataNode *node);
    DataNode *selectStores(DataNode *node);
};

void *_MockAPIMainThread(void *mockAPI);
#endif