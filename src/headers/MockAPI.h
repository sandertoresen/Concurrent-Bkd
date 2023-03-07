#ifndef MOCK_API_THREAD_H
#define MOCK_API_THREAD_H
#include <atomic>
#include "Config.h"
#include "MemoryStructures.h"

struct AverageRun
{
    int index = 0;
    bool full = false;
    float runs[AVG_POOL];
    float average = 0;
};

struct APIWriteNode
{
    atomic<int> containsDataFlag;
    atomic<int> *wait;
    DataNode value;
};

void __calculate_average(AverageRun *avg, float newTime);

class MockApi
{
public:
    MockApi();
    MockApi(char *FILE);
    MockApi(int mockSize);
    ~MockApi();

    DataNode *mockData = nullptr;
    atomic<int> mockDataPtr = 0;

    APIWriteNode APINodeArray[API_WRITERS];

    int generateMockData();
    int loadMockData(char *FILE);

    DataNode *fetchRandom(DataNode *node);
};
#endif