#ifndef MOCK_API_H
#define MOCK_API_H
#include "Config.h"
#include "MemoryStructures.h"

class MockApi
{
public:
    MockApi();
    ~MockApi();

    DataNode *fetchData(int threadNum);

private:
    DataNode *mockData[NUM_THREADS];
    int count[NUM_THREADS];

    int generateMockData();

    int loadMockData(char *FILE);
};

#endif