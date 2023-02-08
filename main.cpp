#include <iostream>
#include "includes/Config.h"
#include "includes/BkdTree.h"
#include "includes/KdbTree.h"
#include <atomic>
#include <string.h>
// #define NUM_NODES 1024

int main()
{
    BkdTree *tree = new BkdTree;

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

    /*for (int i = 0; i < 8; i++)
    {
        _threadInserter(tree);
    }

    printf("Tree size %d\n", tree->globalDiskSize.load());
    for (int i = 0; i < tree->globalDiskSize.load(); i++)
    {
        printf("|%d|(%f,%f):%s\n", i,
               tree->globalDisk[i].cordinates[0],
               tree->globalDisk[i].cordinates[1],
               tree->globalDisk[i].location);
    }
    */
    delete tree;
    // delete tree;
    pthread_exit(NULL);
}

// TODO sjekk ut korrekt construksjon og destruksjon av klasser
