#include <algorithm>
#include <string.h>
#include <iostream>
#include "headers/Config.h"
#include "headers/MockAPI.h"
#include "headers/MemoryStructures.h"

class dataNodeCMP
{
    int dimension;

public:
    dataNodeCMP(int dim) : dimension(dim) {}

    // This operator overloading enables calling
    // operator function () on objects of increment
    bool operator()(DataNode &a, DataNode &b)
    {
        return (bool)(a.cordinates[dimension] < b.cordinates[dimension]);
    }
};

inline void __randomLocation(char *location)
{
    for (int i = 0; i < CHARACTER_LIMIT - 1; i++)
    {
        location[i] = 'a' + rand() % 26;
    }
    location[CHARACTER_LIMIT - 1] = '\0';
}

inline float __randomFloat(float range)
{
    return static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / range));
}

MockApi::MockApi(int valuesPerThread)
{

    dataPerThread = valuesPerThread;
    mockData = new DataNode[dataPerThread * NUM_THREADS];
    fill_n(threadCounter, NUM_THREADS, 0);

    for (int i = 0; i < dataPerThread * NUM_THREADS; i++)
    {
        mockData[i].cordinates[0] = __randomFloat(1000);
        mockData[i].cordinates[1] = __randomFloat(1000);
        __randomLocation(mockData[i].location);
    }

    for (int i = 0; i < dataPerThread; i++)
    {
        printf("(%f,%f):%s\n", mockData[i].cordinates[0], mockData[i].cordinates[1], mockData[i].location);
    }
}

MockApi::MockApi()
{
    fill_n(threadCounter, NUM_THREADS, 0);
}

DataNode *MockApi::fetchData(int threadNum)
{
    if (threadCounter[threadNum] >= dataPerThread)
        return nullptr;

    return &mockData[threadCounter[threadNum]++];
}

DataNode *MockApi::fetchRandom(DataNode *node)
{
    node->cordinates[0] = __randomFloat(1000);
    node->cordinates[1] = __randomFloat(1000);
    __randomLocation(node->location);

    return node;
}

MockApi::~MockApi()
{
    if (mockData != nullptr)
    {
        delete[] mockData;
    }
}
