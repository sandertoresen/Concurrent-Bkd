#ifndef MOCK_API_H
#define MOCK_API_H
#include "Config.h"
#include "MemoryStructures.h"

class MockApi
{
public:
    MockApi();
    MockApi(int valuesPerThread);
    ~MockApi();

    DataNode *fetchData(int threadNum);
    DataNode *fetchRandom(DataNode *node);
    void insertData(DataNode *data, int size);

    // private:
    DataNode *mockData;
    int dataPerThread;
    int threadCounter[NUM_THREADS];

    int generateMockData();
    int loadMockData(char *FILE);
};

#endif