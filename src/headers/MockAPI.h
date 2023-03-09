#ifndef MOCK_API_THREAD_H
#define MOCK_API_THREAD_H
#include <atomic>
#include <list>
#include "Config.h"
#include "MemoryStructures.h"

using namespace std;

struct APIWriteNode
{
    atomic<int> containsDataFlag = -2;
    atomic<int> *wait;
    DataNode value;
};

class Scheduler;

class MockApi
{
public:
    MockApi();
    MockApi(char *FILE, Scheduler *sch);
    MockApi(int mockSize, Scheduler *sch);
    ~MockApi();

    Scheduler *scheduler;
    DataNode *mockData = nullptr;
    atomic<int> mockDataPtr = 0;

    // APIWriteNode APINodeArray[API_WRITERS];
    list<APIWriteNode *> writers;

    int generateMockData();
    int loadMockData(char *FILE);

    DataNode *fetchRandom(DataNode *node);
};

void *_MockAPIMainThread(void *mockAPI);

void _MockAPIRequestInsert(void *mockAPI);

void _MockAPIWrite(void *mockAPI);
#endif