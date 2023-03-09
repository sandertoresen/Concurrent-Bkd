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
    // TODO Setup
    bkdTree = new BkdTree;
    API = new MockApi();
}

void *_schedulerMainThread(void *scheduler)
{
    Scheduler *sch = (Scheduler *)scheduler;

    bool running = true;
    // spawn initial writer threads
    for (int i = 0; i < INITIAL_WRITERS; i++)
    {
        WriterThread *thread = new WriterThread;
        thread->flag = 1;
        thread->tree = sch->bkdTree;
        pthread_create(&thread->thread, nullptr, _threadInserter, (void *)thread->tree);
        sch->activeThreads++;
        sch->writers.push_back(thread);
    }

    while (running)
    {

        // check performance and if more threads are needed(?)

        // perform later bulkloadings

        // cleanup old trees/scheduler maps..

        // Simulate workflow by changing API->delay
    }
    pthread_exit(nullptr);
}