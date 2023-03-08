#include "headers/BkdTree.h"
#include "headers/MockAPI.h"
#include "headers/scheduler.h"
#include <atomic>
#include <algorithm>

using namespace std;

Scheduler::Scheduler()
{
    fill_n(writeQueueAPI, API_WRITE_QUEUE_SIZE, nullptr);
    waitingAPIs = new atomic<int>(0);
    // TODO Setup
    bkdTree = new BkdTree;
}

void Scheduler::checkWriteQueue()
{
    for (int i = 0; i < API_WRITE_QUEUE_SIZE; i++)
    {
        APIWriteNode *api = writeQueueAPI[i].load();
        if (api == nullptr)
        {
            continue;
        }
        api->wait = waitingAPIs;

        // assign the api to a thread..
        /*
        if thread aveilable:
            assign
        if no thread aveilable:
            schedule new thread
        */
    }
}

void *_schedulerMainThread(void *scheduler)
{
    Scheduler *sch = (Scheduler *)scheduler;

    sch->checkWriteQueue();
}