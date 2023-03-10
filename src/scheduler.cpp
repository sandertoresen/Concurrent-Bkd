#include <algorithm>
#include <atomic>
#include <chrono>
#include <pthread.h>
#include <iostream>
#include <unordered_map>
#include "headers/BkdTree.h"
#include "headers/KdbTree.h"
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
        ScheduledThread *thread = new ScheduledThread;
        thread->flag = 1;
        thread->tree = sch->bkdTree;
        pthread_create(&thread->thread, nullptr, _threadInserterControlled, (void *)thread);
        sch->activeThreads++;
        sch->writers.push_back(thread);
    }

    for (int i = 0; i < INITIAL_READERS; i++)
    {
        ScheduledThread *thread = new ScheduledThread;
        thread->flag = 1;
        thread->tree = sch->bkdTree;
        pthread_create(&thread->thread, nullptr, _windowLookup, (void *)thread);
        sch->activeThreads++;
        sch->readers.push_back(thread);
    }

    int runningTime = 5;
    auto start_time = std::chrono::steady_clock::now();
    while (running)
    {
        if (std::chrono::steady_clock::now() - start_time > std::chrono::seconds(runningTime))
        {

            printf("Got %d trees\n", sch->bkdTree->treeId.load());
            for (auto it = sch->writers.begin(); it != sch->writers.end(); it++)
            {
                printf("Shut down thread\n");
                ScheduledThread *t = *it;
                t->flag.store(0);
            }

            for (auto it = sch->readers.begin(); it != sch->readers.end(); it++)
            {
                printf("Shut down thread\n");
                ScheduledThread *t = *it;
                t->flag.store(0);
            }

            break;
        }

        sch->deleteOldMaps();
    }
    // check performance and if more threads are needed(?)

    // perform later bulkloadings

    // cleanup old trees/scheduler maps..

    // Simulate workflow by changing API->delay

    pthread_exit(nullptr);
}

void Scheduler::deleteOldMaps()
{
    // schedulerDeletedMaps[i]
    // bkdTree->schedulerDeletedMaps
    for (int i = 0; i < SCHEDULER_MAP_ARRAY_SIZE; i++)
    {
        AtomicUnorderedMapElement *deleteMap = bkdTree->schedulerDeletedMaps[i].load();
        if (deleteMap == nullptr)
        {
            continue;
        }
        if (deleteMap->readers.load() != 0)
        {
            continue;
        }

        // For each map here delete them if flagged as deleted and have no readers..
        //  deleteMap->readableTrees
        for (const auto &[_, value] : *deleteMap->readableTrees)
        {
            if (!value->deleted)
            {
                continue;
            }
            KdbDestroyTree(value->tree);

            bkdTree->schedulerDeletedMaps[i].store(nullptr);
        }
        delete deleteMap->readableTrees;
        delete deleteMap;

        bkdTree->schedulerDeletedMaps[i].store(nullptr);
    }
}