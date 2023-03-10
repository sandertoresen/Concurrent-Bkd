#include <algorithm>
#include <chrono>
#include <string.h>
#include <iostream>
#include "headers/Config.h"
#include "headers/MockAPI.h"
#include "headers/MemoryStructures.h"
#include "headers/scheduler.h"

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
    delay = API_DELAY_MS;
}

MockApi::MockApi(int mockSize)
{
    mockData = new DataNode[mockSize];
    delay = API_DELAY_MS;
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
    auto start_time = std::chrono::steady_clock::now();
    while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count() < delay)
    {
    }

    return node;
}

DataNode *MockApi::selectStores(DataNode *node)
{
    int counter = mockDataPtr.fetch_add(1);
    node->cordinates[0] = mockData->cordinates[0];
    node->cordinates[1] = mockData->cordinates[1];
    strcpy(node->location, mockData[counter].location);
    auto start_time = std::chrono::steady_clock::now();
    while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count() < delay)
    {
    }

    return node;
}

WindowQuery *MockApi::fetchWindowQuery()
{
    WindowQuery *query = new WindowQuery;
    for (int d = 0; d < DIMENSIONS; d++)
    {
        float a = 0;    //__randomFloat(1000);
        float b = 1000; //__randomFloat(1000);
        if (a > b)
        {
            query->window[d][0] = b;
            query->window[d][1] = a;
        }
        else
        {
            query->window[d][0] = a;
            query->window[d][1] = b;
        }
    }

    auto start_time = std::chrono::steady_clock::now();
    while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count() < delay)
    {
    }

    return query;
}

MockApi::~MockApi()
{
    if (mockData != nullptr)
    {
        delete[] mockData;
    }
}