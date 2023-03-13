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
}
Scheduler::~Scheduler()
{
    delete bkdTree;
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
    while (running)
    {

        if (sch->bkdTree->treeId.load() > 40)
        {
            printf("Got %d trees\n", sch->bkdTree->treeId.load());
            sch->shutdown();
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

void Scheduler::shutdown()
{
    for (auto it = writers.begin(); it != writers.end(); it++)
    {
        printf("Shut down write thread\n");
        ScheduledThread *t = *it;
        t->flag.store(0);
    }

    for (auto it = readers.begin(); it != readers.end(); it++)
    {
        printf("Shut down read thread\n");
        ScheduledThread *t = *it;
        t->flag.store(0);
    }

    for (auto it = writers.begin(); it != writers.end();)
    {
        ScheduledThread *t = *it;
        while (t->flag.load() != -1)
        {
        }
        it = writers.erase(it);
        // delete t;
    }

    for (auto it = readers.begin(); it != readers.end();)
    {
        ScheduledThread *t = *it;
        while (t->flag.load() != -1)
        {
        }
        it = readers.erase(it);
        // delete t;
    }
    printf("threads erased!\n");
}

void Scheduler::largeBulkloads()
{
    // TODO: include bloomfilter
}
