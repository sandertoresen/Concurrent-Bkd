#include <algorithm>
#include <chrono>
#include <random>
#include <string.h>
#include <iostream>
#include "headers/Config.h"
#include "headers/MockAPI.h"
#include "headers/MemoryStructures.h"
#include "headers/scheduler.h"

inline void __randomLocation(char *location)
{
    static random_device rd;
    static thread_local mt19937 gen(rd());
    static thread_local uniform_int_distribution<> dis('a', 'z');

    for (int i = 0; i < CHARACTER_LIMIT - 1; i++)
    {
        location[i] = static_cast<char>(dis(gen));
    }
    location[CHARACTER_LIMIT - 1] = '\0';
}

inline float __randomUniformDistribution(float x, float y)
{
    static thread_local mt19937 generator(random_device{}());
    thread_local uniform_real_distribution<float> distribution(x, y);
    return distribution(generator);
}

inline float __randomNormalDistribution(float mean, float stddev)
{
    static thread_local mt19937 generator(random_device{}());
    thread_local normal_distribution<float> distribution(mean, stddev);
    return distribution(generator);
}

MockApi::MockApi::MockApi()
{
    min = API_MIN;
    max = API_MAX;
    delay = API_DELAY_MS;
    if (API_DATA_TYPE == 0)
    {
        selectedRandomFunc = &__randomUniformDistribution;
    }
    if (API_DATA_TYPE == 1)
    {
        selectedRandomFunc = &__randomNormalDistribution;
    }
}

DataNode *MockApi::fetchRandom(DataNode *node)
{
    node->cordinates[0] = selectedRandomFunc(min, max);
    node->cordinates[1] = selectedRandomFunc(min, max);

    __randomLocation(node->location);
    auto start_time = chrono::steady_clock::now();
    while (chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start_time).count() < delay)
    {
    }

    return node;
}

WindowQuery *MockApi::fetchWindowQuery()
{
    WindowQuery *query = new WindowQuery;
    for (int d = 0; d < DIMENSIONS; d++)
    {
        float a = 0;      //__randomFloat(1000);
        float b = 100000; //__randomFloat(1000);
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

    auto start_time = chrono::steady_clock::now();
    while (chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start_time).count() < delay)
    {
    }

    return query;
}

MockApi::~MockApi()
{
}