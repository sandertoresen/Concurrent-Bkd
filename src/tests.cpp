#include <string.h>
#include <iostream>
#include <algorithm>
#include "headers/tests.h"
#include "headers/KdbTree.h"
#include "headers/MemoryStructures.h"
#include "headers/MockAPI.h"

using namespace std;

class dataNodeCMP
{
    int dimension;

public:
    dataNodeCMP(int dim) : dimension(dim) {}

    // This operator overloading enables calling
    // operator function () on objects of increment
    bool operator()(DataNode &a, DataNode &b)
    {
        return (bool)(a.cordinates[dimension] < b.cordinates[dimension]);
    }
};

void _test_kdb_tree_memory_leak()
{
    int numNodes = 8;
    DataNode *values = new DataNode[numNodes * DIMENSIONS];
    MockApi *API = new MockApi;

    int offset = 0;
    DataNode *value = new DataNode;
    API->fetchRandom(value);
    for (int i = 0; i < numNodes; i++)
    {
        // API->fetchRandom(&values[i]);
        values[i].cordinates[0] = value->cordinates[0];
        values[i].cordinates[1] = value->cordinates[1];

        strcpy(values[i].location, value->location);
    }

    for (int d = 0; d < DIMENSIONS; d++)
    {
        if (d != 0) // paste over tree data
            memcpy(&values[d * numNodes], &values[0], sizeof(DataNode) * numNodes);

        sort(&values[d * numNodes], &values[d * numNodes + numNodes], dataNodeCMP(d));
    }

    KdbTree *tree = KdbCreateTree(values, numNodes, 42);
    printf("created tree!\n");
    // Fetch nodes back
    DataNode *leakTest = new DataNode[numNodes];
    KdbTreeFetchNodes(tree, leakTest);
    // for (int i = 0; i < numNodes; i++)
    // {
    //     printf("(%d,%d):%s\n", leakTest->cordinates[0], leakTest->cordinates[1], leakTest->location);
    // }

    KdbDestroyTree(tree);
    printf("task exited succ\n");
    delete[] leakTest;
    delete[] values;
    delete value;
    delete API;
}