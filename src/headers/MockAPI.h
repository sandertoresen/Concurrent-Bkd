#ifndef MOCK_API_THREAD_H
#define MOCK_API_THREAD_H
#include <atomic>
#include "Config.h"
#include "MemoryStructures.h"

struct APIWriteNode
{
    atomic<int> flag;
    atomic<int> *wait;
    DataNode *value;
};

struct AverageRun
{
    int index = 0;
    bool full = false;
    int size = 5;
    float runs[50];
    float average = 0;
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