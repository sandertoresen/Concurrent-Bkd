#ifndef KDB_TREE_H
#define KDB_TREE_H

#include <list>
#include <atomic>
#include "Config.h"
#include "MemoryStructures.h"

// Branch node traversing tree
struct KdbBranch
{
    float treeSplit; // value splitting the tree
    int dimension;
    KdbBranch *left = nullptr;
    KdbBranch *right = nullptr;

    KdbBranch *leafLeft = nullptr; // connect leafs through list    // if leaf != nullptr, the tree has been traversed
    int leafSize = KDB_LEAF_SIZE;  // add leafsize
    DataNode *KdbLeafBuffer = nullptr;
};

// KDB tree root node
struct KdbTree
{
    float treeSplit = -1;
    int size = 0;
    int level;
    long id;
    atomic<bool> deleted = false;
    KdbBranch *left;
    KdbBranch *right;

    KdbBranch *leafList = nullptr; // pointer to leaf branch linked list
};

int __cordEqual(float c1[DIMENSIONS], float c2[DIMENSIONS]);

void __printDataNode(DataNode *ptr);

void __printKdbBranch(KdbBranch *branch);
void __printKdbTree(KdbTree *tree);

void __printLeafs(KdbTree *tree);

void KdbTreeRangeSearch(KdbTree *tree, float range[DIMENSIONS][2], std::list<DataNode> &values);

void _KdbBranchRangeSearch(KdbBranch *branch, float range[DIMENSIONS][2], int currentDimension, std::list<DataNode> &values);

void KdbTreeFetchNodes(KdbTree *tree, DataNode *values);

int KdbTreeRemove(KdbTree *tree, float index[DIMENSIONS]);

int _KdbBranchRemove(KdbBranch *branch, float index[DIMENSIONS]);

KdbBranch *KdbCreateBranch(DataNode *values, int numNodes, KdbTree *root, KdbBranch *parent, int currentDimension);

KdbTree *KdbCreateTree(DataNode *values, int numNodes, long treeId, int level);

void KdbDestroyTree(KdbTree *tree);

void _KdbDestroyBranch(KdbBranch *branch);

#endif

// OBS.. Each KDB_Branch should probably keep track of the nr. of branches and not assume KDB_LEAF_SIZE