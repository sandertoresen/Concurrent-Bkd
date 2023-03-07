#include <algorithm>
#include <string.h>
#include <iostream>
#include "headers/Config.h"
#include "headers/MockAPI.h"
#include "headers/MemoryStructures.h"

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

MockApi::MockApi::MockApi()
{
}

MockApi::MockApi(int mockSize)
{

    mockData = new DataNode[mockSize];

    for (int i = 0; i < mockSize; i++)
    {
        mockData[i].cordinates[0] = __randomFloat(1000);
        mockData[i].cordinates[1] = __randomFloat(1000);
        __randomLocation(mockData[i].location);
    }
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

void __calculate_average(AverageRun *avg, float newTime)
{
    if (!avg->full)
    {
        avg->runs[avg->index++] = newTime;
        avg->average = 0;
        for (int i = 0; i < avg->index; i++)
        {
            avg->average += avg->runs[i] / avg->index;
        }

        if (avg->index == AVG_POOL)
        {
            avg->full = true;
        }
    }

    else
    {
        int lastVal = (avg->index - 1) % AVG_POOL;

        avg->average -= avg->average - (avg->runs[lastVal] / AVG_POOL);
        // new first val
        avg->runs[lastVal] = newTime;
        avg->index = lastVal;
        avg->average += avg->average + (avg->runs[lastVal] / AVG_POOL);
    }
}