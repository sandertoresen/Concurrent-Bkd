#ifndef SCHEDULER
#define SCHEDULER
#include <atomic>
#include <pthread.h>
#include <unordered_set>
#include "ThreadFunctions.h"
#include "BkdTree.h"
#include "MockAPI.h"
// The scheduler should be responsible for running the whole structure

/*
Responsibilities:
 - cleaning structure
 - managing threads
 - time different functions
    -> if bulkloading becomes a bottleneck, lower MAX_BULKLOAD_LEVEL

 - keeping track of resources and resource usage
    -> how much resources should be assigned to each task?
    -> if low resource usage assign more threads to read operations?
    -> if low resource usage perform larger bulkload operations
        -> log high/low usagetimes


 - large schedule bulkloader
    -> large bulkloader responsible for deleting nodes



 - deletion using bloomfilters
    -> readers needs to spot deleted nodes
        -> could delete them from the list or delete them direclty from the tree when reading it


Workflow: API -> header node -> queue to a work node -> work node return -> API

Se på writeThreads som ports accepting data isj
Send data:
- assume active inserter, just contact it directly(in a real world scenario this could be a thread dedicated to listening for requests and performin them)
- best with dedicated readers and writers

Writers:
//flag set by scheduler (FILL: scheduler should cancel job preemtivly as threads will still need to finish current buffer)
while(continueWriteFlag)
   // should be both insert and delete
   _threadInsert()

while(continueReadFlag)
   _threadRead()

Need a mechanism for knowing if the current readers/writers is enough to conquer demand:
- queue:
   - if i am a API and I'm waiting for a response, increment a queue counter

Scheduler is responsible for assigning spesific APIs to spesific threads..
*/

class Scheduler
{
public:
   Scheduler();
   ~Scheduler();
   void deleteOldMaps();
   void shutdown();

   /*
   Perform bulkloads of given level size, combines largest 2^n nr. of trees of a selected level
   */
   void largeBulkloads(int selectedLevel);
   BkdTree *bkdTree;
   int maxThreads;
   atomic<int> activeThreads;

   list<ScheduledThread *> writers;
   list<ScheduledThread *> readers;
   BulkLoadThread *bulkLoader = nullptr;
};

void *_schedulerMainThread(void *scheduler);

#endif