#include <iostream>
#include "includes/Config.h"
#include "includes/BkdTree.h"
#include <atomic>

#define NUM_THREADS 7

int main()
{
    BkdTree *tree = new BkdTree;

    // // destructors is automatically called when delete is called
    // delete tree;

    pthread_t threads[NUM_THREADS];
    int rc;
    int i;

    for (i = 0; i < NUM_THREADS; i++)
    {
        //   cout << "main() : creating thread, " << i << endl;
        rc = pthread_create(&threads[i], NULL, _threadInserter, (void *)tree);

        if (rc)
        {
            cout << "Error:unable to create thread," << rc << endl;
            exit(-1);
        }
        pthread_join(threads[i], NULL);
    }

    printf("Tree size %d\n", tree->globalMemorySize.load());
    // for (int i = 0; i < tree->globalMemorySize.load(); i++)
    // {
    //     printf("|%d|(%f,%f):%s\n", i,
    //            tree->globalMemory[i].cordinates[0],
    //            tree->globalMemory[i].cordinates[1],
    //            tree->globalMemory[i].location);
    // }

    pthread_exit(NULL);
}

// TODO sjekk ut korrekt construksjon og destruksjon av klasser
