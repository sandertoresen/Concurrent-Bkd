#include <iostream>
// #include "includes/tests.h"
#include "src/headers/scheduler.h"
#include "src/headers/MockAPI.h"
#include "src/headers/Config.h"
#include "src/headers/BkdTree.h"
#include "src/headers/ThreadFunctions.h"

int main()
{
    pthread_t thread;
    Scheduler *scheduler = new Scheduler;
    pthread_create(&thread, nullptr, _schedulerMainThread, (void *)scheduler);
    pthread_join(thread, nullptr);
}

/*
IDEA:
simmulate data by watching how ofthen each thread inserts data blocks!
    -> slipper da og bruke MockAPI thread som sakker ned løsningen og krever mer synkronisering!
    -> MockAPI responsible for:
        - setting random timeout between inserts
        - supplying data read by threads



Todo:
- How does thread manager work with Read/Write?
-   -> runs by setting the test workload in MockAPI
- How to implement Read in a smart manner?
-   -> if low workflow use multiple threads to search(?)
-   -> readers have safe structures to read from
-   -> best to have readers only accessing trees(is already a delayed storage, point to slow speeds when accessing arrays)
-   -> only accessing trees will also allow for large array sizes without bottlenecking performance, this is beneficially both for threads and not having to iterate over millions of entries in O(N)

// discussion data age(only read most recent entries(?))


- How does thread manager spot how much work there is?
-   -> How long a given thread uses before it exits after inserting
-   -> Use same measurement to figure out when to start bulkloadings(discussion point)

- Tombstone deletion method with bloomfilters
-   -> bulkloading concurrent system where nodes can quickly set values to deleted
- Reusing memory structures(?)(discussion point, perform tree creation with tree creation with and without memory allocation)


BULKLOADING STRATEGY:
- Lock entire bulkload, if a thread is locked for a long period,
data input is so high that smaller bulkloadings should be performed



Discussion/fill
- Talk about BkdTree beeing made to be thread safe and is just a vessel for threads to work on
- syncronization methods. (atomic defaults to locks if underlying architecture dont support atomic)
- thread workload. (how large must the workload be to overcome startup (talk about gustavsons law))
    -> test with generic example and with BkdTree implementation

// FILL: use struct to don't point direcly at list, can then iterate over without having issues of data changing(?) isj

FILL: test how much overhead is created by spawning thread and thereby how many inserts each thread should perform to be worth spawning
    -> this could be decided by the scheduler(?)

FILL: a sellingpoint in the original BKD tree was the low storage overhead, due to beeing a more complex structure, the concurrent version adds more overhead compared to the original solution

FILL: the implementation could be more efficiend for example all the sorting of every dimension, using a more efficient bulking strategy,
IDEAS:
-   use Type cordinates[DIMENSIONS]



FILL: considered having the last reader delete the struct, but to keep reader performance consistent, this task was assigned to the scheduler

*/

/*for (int i = 0; i < numNodes; i++)
    {
        printf("|%d|(%f,%f):%s\n", i,
               values[i].cordinates[0],
               values[i].cordinates[1],
               values[i].location);
    }*/