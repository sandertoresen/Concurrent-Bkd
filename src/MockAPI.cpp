#include <algorithm>
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
}

MockApi::MockApi(int mockSize, Scheduler *sch)
{

    mockData = new DataNode[mockSize];

    for (int i = 0; i < mockSize; i++)
    {
        mockData[i].cordinates[0] = __randomFloat(1000);
        mockData[i].cordinates[1] = __randomFloat(1000);
        __randomLocation(mockData[i].location);
    }
    scheduler = sch;
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

void *_MockAPIMainThread(void *mockAPI)
{
    // request a given amount of write APIs
    const int spawnAPIs = 1;
    const int insertAPIs = THREAD_BUFFER_SIZE;

    for (int i = 0; i < spawnAPIs; i++)
    {
        _MockAPIRequestInsert(mockAPI);
    }

    for (int i = 0; i < insertAPIs; i++)
    {
        _MockAPIWrite(mockAPI);
    }

    pthread_exit(nullptr);
}

void _MockAPIRequestInsert(void *mockAPI)
{
    MockApi *API = (MockApi *)mockAPI;
    Scheduler *scheduler = API->scheduler;
    APIWriteNode *apiNode = new APIWriteNode;
    API->writers.push_back(apiNode);
    APIWriteNode *expected = nullptr;
    bool inserted = false;
    while (!inserted)
    {
        for (int i = 0; i < API_WRITE_QUEUE_SIZE; i++)
        {
            if (scheduler->writeQueueAPI[i].compare_exchange_strong(expected, apiNode))
            {
                printf("inserted writeAPI!\n");
                inserted = true;
                break;
            }
        }
    }
}

void _MockAPIWrite(void *mockAPI)
{
    MockApi *API = (MockApi *)mockAPI;
    while (API->mockDataPtr.load() < THREAD_BUFFER_SIZE)
    {
        for (auto it = API->writers.begin(); it != API->writers.end(); ++it)
            for (int i = 0; i < API_WRITERS; i++)
            {
                APIWriteNode *api = *it;
                int containsDataFlag = api->containsDataFlag.load();
                if (containsDataFlag == 1 || containsDataFlag == -2)
                    continue;

                int counter = API->mockDataPtr.fetch_add(1);

                printf("API:Sent data\n");
                api->value = API->mockData[counter];
                api->containsDataFlag.store(1);

                api->wait++;
            }
    }
}
