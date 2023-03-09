#include <algorithm>
#include <atomic>
#include <pthread.h>
#include <iostream>
#include "headers/BkdTree.h"
#include "headers/MockAPI.h"
#include "headers/scheduler.h"

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

        if (schedulerWriters.empty())
        {
            WriterThread *writerThread = new WriterThread;
            writerThread->tree = bkdTree;
            writerThread->numAPIs = 1;
            fill_n(writerThread->nodes, API_MAX_WRITER, nullptr);
            writerThread->nodes[0] = api;
            schedulerWriters.push_back(writerThread);
            pthread_create(&writerThread->thread, nullptr, _threadInserterApi, (void *)writerThread);

            api->containsDataFlag = -1;
        }
        /*
        assign the api to a thread..
        spawn new threads if aveilable(?)
         -> how should resources be distributed
         -> should there be extras, mabye one reader thread(?)
        if slow redistribute resources

        have some threads aveilable for performing larger bulkloads, if reads come in,
        stop bulkloading temporarily and answer the query(? )

        if thread aveilable:
            assign
        if no thread aveilable:
            schedule new thread
        if no more room for api:
            extend thread array and insert
        */
    }
}

void *_schedulerMainThread(void *scheduler)
{
    Scheduler *sch = (Scheduler *)scheduler;

    bool running = true;
    while (running)
    {
        sch->checkWriteQueue();

        // check performance and if more threads are needed(?)

        // perform later bulkloadings

        // cleanup old trees..
    }
    pthread_exit(nullptr);
}