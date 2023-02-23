#include <iostream>
#include "includes/Config.h"
#include "includes/BkdTree.h"
#include "includes/KdbTree.h"
#include "includes/MockAPI.h"
#include "includes/MemoryStructures.h"
#include "includes/ThreadStructures.h"
#include <atomic>
#include <string.h>

void *_windowLookup(void *input)
{
    // TODO: start with single thread looking for all data

    windowLookupInput *data = (windowLookupInput *)input;
    DataNode *val = new DataNode;
    strcpy(val->location, "Input");
    data->results.push_back(val);

    /*
    - lookup global mem
    - lookup global disk
    - lookup first tree list
    - tree list of N size
    - tree list of N++ sizes

    - Which way should i read(?)
    new -> old?
    old -> new?

    make structures read safe with atomic flags

    assert i don't add duplicates to result list?
    assert values are not deleted

    */

    pthread_exit(NULL);
}

int main()
{
    BkdTree *tree = new BkdTree;

    windowLookupInput *input = new windowLookupInput;
    input->tree = tree;
    for (int d = 0; d < DIMENSIONS; d++)
    {
        input->window[d][0] = 0;
        input->window[d][1] = 1000;
    }

    pthread_t threads[NUM_THREADS];
    int rc;
    int i;

    for (i = 0; i < NUM_THREADS; i++)
    {
        //   cout << "main() : creating thread, " << i << endl;
        // rc = pthread_create(&threads[i], NULL, _threadInserter, (void *)tree);
        rc = pthread_create(&threads[i], NULL, _windowLookup, (void *)input);
        if (rc)
        {
            cout << "Error:unable to create thread," << rc << endl;
            exit(-1);
        }
        pthread_join(threads[i], NULL);
    }

    for (std::list<DataNode *>::iterator itr = input->results.begin(); itr != input->results.end(); ++itr)
    {
        DataNode *tmp = *itr;
        printf("God: %s\n", tmp->location);
    }
    delete tree;
    pthread_exit(NULL);
    // delete tree;
}

/*
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

IDEAS:
-   use Type cordinates[DIMENSIONS]


*/

/*for (int i = 0; i < numNodes; i++)
    {
        printf("|%d|(%f,%f):%s\n", i,
               values[i].cordinates[0],
               values[i].cordinates[1],
               values[i].location);
    }*/