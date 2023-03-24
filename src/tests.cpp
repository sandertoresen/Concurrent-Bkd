#include <string.h>
#include <iostream>
#include <algorithm>
#include "headers/tests.h"
#include "headers/KdbTree.h"
#include "headers/MemoryStructures.h"
#include "headers/MockAPI.h"
#include "headers/BkdTree.h"

using namespace std;

void _test_kdb_tree_memory_leak()
{
    int numNodes = 1024;
    DataNode *values = new DataNode[numNodes * DIMENSIONS];
    MockApi *API = new MockApi;

    int offset = 0;
    DataNode *value = new DataNode;
    for (int i = 0; i < numNodes; i++)
    {
        API->fetchRandom(&values[i]);

        values[i].cordinates[0] = 1; // value->cordinates[0];
        values[i].cordinates[1] = 2; // value->cordinates[1];
    }

    for (int d = 0; d < DIMENSIONS; d++)
    {
        if (d != 0) // paste over tree data
            memcpy(&values[d * numNodes], &values[0], sizeof(DataNode) * numNodes);

        sort(&values[d * numNodes], &values[d * numNodes + numNodes], dataNodeCMP(d));
    }

    KdbTree *tree = KdbCreateTree(values, numNodes, 42, -1);
    printf("created tree!\n");
    // Fetch nodes back
    DataNode *leakTest = new DataNode[numNodes];
    KdbTreeFetchNodes(tree, leakTest);

    for (int i = 0; i < numNodes * DIMENSIONS; i++)
    {
        bool found = false;
        for (int j = 0; j < numNodes; j++)
        {
            if (__cordEqual(values[i].cordinates, leakTest[j].cordinates) && strcmp(values[i].location, leakTest[j].location) == 0)
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            printf("Didn't find one node!!\n");
        }
    }

    KdbDestroyTree(tree);
    printf("task exited succ\n");
    delete[] leakTest;
    delete[] values;
    delete value;
    delete API;
}