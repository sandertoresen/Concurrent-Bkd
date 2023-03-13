#include <iostream>
#include <string.h>
#include <list>
#include "headers/Config.h"
#include "headers/KdbTree.h"

void __printKdbBranch(KdbBranch *branch)
{
    if (!branch)
    {
        printf("bottom\n");
    }
    if (branch->KdbLeafBuffer != nullptr)
    {
        if (KDB_LEAF_SIZE == 1)
        {
            printf("Leaf : ");
            for (int d = 0; d < DIMENSIONS; d++)
            {
                if (!d)
                    printf("[(%d, ", branch->KdbLeafBuffer[0].cordinates[0]);
                else if (d + 1 == DIMENSIONS)
                    printf("%d) data:%s]\n", branch->KdbLeafBuffer[0].cordinates[d], branch->KdbLeafBuffer[0].location);
                else
                    printf("%d, ", branch->KdbLeafBuffer[0].cordinates[d]);
            }
            return;
        }
        printf("Leafs : ");
        for (int i = 0; i < KDB_LEAF_SIZE; i++)
        {
            for (int d = 0; d < DIMENSIONS; d++)
            {
                if (!d)
                    printf("[(%d, ", branch->KdbLeafBuffer[i].cordinates[d]);
                else if (d + 1 == DIMENSIONS)
                    printf("%d) data:%s] ", branch->KdbLeafBuffer[i].cordinates[d], branch->KdbLeafBuffer[i].location);
                else
                    printf("%d, ", branch->KdbLeafBuffer[i].cordinates[d]);
            }
        }
        printf("\n");

        return;
    }

    __printKdbBranch(branch->left);
    __printKdbBranch(branch->right);
}

void __printKdbTree(KdbTree *tree)
{
    __printKdbBranch(tree->left);
    __printKdbBranch(tree->right);
}

void __printLeafs(KdbTree *tree)
{
    KdbBranch *ptr = tree->leafList;
    while (ptr != nullptr)
    {
        if (KDB_LEAF_SIZE == 1)
        {
            printf("Leaf : ");
            for (int d = 0; d < DIMENSIONS; d++)
            {
                if (!d)
                    printf("[(%d, ", ptr->KdbLeafBuffer[0].cordinates[0]);
                else if (d + 1 == DIMENSIONS)
                    printf("%d) data:%s]\n", ptr->KdbLeafBuffer[0].cordinates[d], ptr->KdbLeafBuffer[0].location);
                else
                    printf("%d, ", ptr->KdbLeafBuffer[0].cordinates[d]);
            }
        }
        else
        {
            printf("Leafs : ");
            for (int i = 0; i < KDB_LEAF_SIZE; i++)
            {
                for (int d = 0; d < DIMENSIONS; d++)
                {
                    if (!d)
                        printf("[(%d, ", ptr->KdbLeafBuffer[i].cordinates[d]);
                    else if (d + 1 == DIMENSIONS)
                        printf("%d) data:%s] ", ptr->KdbLeafBuffer[i].cordinates[d], ptr->KdbLeafBuffer[i].location);
                    else
                        printf("%d, ", ptr->KdbLeafBuffer[i].cordinates[d]);
                }
            }
            printf("\n");
        }
        ptr = ptr->leafLeft;
    }
}

int __cordEqual(float c1[DIMENSIONS], float c2[DIMENSIONS])
{
    for (int i = 0; i < DIMENSIONS; i++)
    {
        if (c1[i] != c2[i])
            return 0;
    }
    return 1;
}

void KdbTreeRangeSearch(KdbTree *tree, float range[DIMENSIONS][2], std::list<DataNode> &values)
{
    if (range[0][0] <= tree->treeSplit)
    {
        _KdbBranchRangeSearch(tree->left, range, 1 % DIMENSIONS, values);
    }

    if (range[0][1] > tree->treeSplit)
    {
        _KdbBranchRangeSearch(tree->right, range, 1 % DIMENSIONS, values);
    }
}

void _KdbBranchRangeSearch(KdbBranch *branch, float range[DIMENSIONS][2], int currentDimension, std::list<DataNode> &values)
{
    if (branch->KdbLeafBuffer != nullptr)
    {
        for (int i = 0; i < branch->leafSize; i++)
        {
            DataNode leafPtr = branch->KdbLeafBuffer[i];
            bool in_range = true;
            for (int d = 0; d < DIMENSIONS; d++)
            {
                // break if cord is outside scope
                if (range[d][0] > leafPtr.cordinates[d] || leafPtr.cordinates[d] > range[d][1])
                {
                    in_range = false;
                    break;
                }
                // if(range[d][0])
            }
            if (in_range)
            {
                values.push_back(leafPtr);
            }
        }
        return;
    }

    if (range[currentDimension][0] <= branch->treeSplit)
    {
        _KdbBranchRangeSearch(branch->left, range, (currentDimension + 1) % DIMENSIONS, values);
    }

    if (range[currentDimension][1] > branch->treeSplit)
    {
        _KdbBranchRangeSearch(branch->right, range, (currentDimension + 1) % DIMENSIONS, values);
    }
}

void KdbTreeFetchNodes(KdbTree *tree, DataNode *values)
{
    KdbBranch *leafBranch = tree->leafList;
    int count = 0;
    while (leafBranch)
    {
        memcpy(&values[count], leafBranch->KdbLeafBuffer, sizeof(DataNode) * leafBranch->leafSize);
        count += leafBranch->leafSize;
        leafBranch = leafBranch->leafLeft;
    }
}

int KdbTreeRemove(KdbTree *tree, float index[DIMENSIONS])
{ // return 0 if node not found, otherwise 1 if succesfull delete
    if (!tree)
        return 0;
    if (index[0] < tree->treeSplit)
    {
        return _KdbBranchRemove(tree->left, index);
    }

    if (index[0] > tree->treeSplit)
    {
        return _KdbBranchRemove(tree->right, index);
    }

    if (index[0] == tree->treeSplit)
    {
        if (_KdbBranchRemove(tree->left, index))
            return 1;
        if (_KdbBranchRemove(tree->right, index))
            return 1;
    }

    return 0;
}

int _KdbBranchRemove(KdbBranch *branch, float index[DIMENSIONS])
{
    if (branch->KdbLeafBuffer != nullptr)
    {
        // looking in leaf:
        for (int i = 0; i < branch->leafSize; i++)
        {
            DataNode tmp = branch->KdbLeafBuffer[i];
            bool match = true;
            if (!__cordEqual(tmp.cordinates, index))
            {
                match = false;
            }
            if (match)
            {
                // overwrite datanode
                // obs we don't write over last node
                memcpy(&branch->KdbLeafBuffer[i], &branch->KdbLeafBuffer[branch->leafSize - 1], sizeof(DataNode));

                branch->leafSize--;
                return 1;
            }
        }
        return 0;
    }

    if (index[branch->dimension] < branch->treeSplit)
    {
        // printf("|%d|Go left %d < %d\n", branch->dimension, index[branch->dimension], branch->treeSplit);
        return _KdbBranchRemove(branch->left, index);
    }

    if (index[branch->dimension] > branch->treeSplit)
    {
        // printf("|%d|Go right %d > %d\n", branch->dimension, index[branch->dimension], branch->treeSplit);
        return _KdbBranchRemove(branch->right, index);
    }

    if (index[branch->dimension] == branch->treeSplit)
    {
        // printf("|%d|Go both %d == %d\n", branch->dimension, index[branch->dimension], branch->treeSplit);
        if (_KdbBranchRemove(branch->left, index))
            return 1;
        if (_KdbBranchRemove(branch->right, index))
            return 1;
    }

    return 0;
}

KdbBranch *KdbCreateBranch(DataNode *values, int numNodes, KdbTree *root, KdbBranch *parent, int currentDimension)
{
    // printf("Branch|numNodes:%d\n", numNodes);
    if (numNodes <= KDB_LEAF_SIZE)
    {

        KdbBranch *leafBranch = new KdbBranch;
        leafBranch->dimension = currentDimension;
        leafBranch->KdbLeafBuffer = new DataNode[KDB_LEAF_SIZE];

        // add root pointer
        if (root->leafList == nullptr)
        {
            root->leafList = leafBranch;
        }
        else
        {
            KdbBranch *ptr = root->leafList;
            while (ptr != nullptr)
            {
                if (ptr->leafLeft == nullptr)
                {
                    ptr->leafLeft = leafBranch;
                    break;
                }
                ptr = ptr->leafLeft;
            }
        }

        memcpy(leafBranch->KdbLeafBuffer, &values[numNodes * currentDimension], sizeof(DataNode) * numNodes);
        return leafBranch;
    }

    KdbBranch *branch = new KdbBranch;

    int leftMiddleIndex = numNodes / 2 - !(numNodes % 2);
    int rightMiddleIndex = leftMiddleIndex + 1;
    int rightSize = rightMiddleIndex - (numNodes % 2);
    branch->treeSplit = (values[numNodes * currentDimension + leftMiddleIndex].cordinates[currentDimension] + values[numNodes * currentDimension + rightMiddleIndex].cordinates[currentDimension]) / 2;
    branch->dimension = currentDimension;

    DataNode *leftDataNodes = new DataNode[rightMiddleIndex * DIMENSIONS];
    DataNode *rightDataNodes = new DataNode[rightSize * DIMENSIONS];

    // fill data
    memcpy(&leftDataNodes[rightMiddleIndex * currentDimension], &values[currentDimension * numNodes], sizeof(DataNode) * rightMiddleIndex);
    memcpy(&rightDataNodes[rightSize * currentDimension], &values[currentDimension * numNodes + rightMiddleIndex], sizeof(DataNode) * rightSize);
    int leftCounter[DIMENSIONS] = {};
    int rightCounter[DIMENSIONS] = {};

    for (int i = 0; i < numNodes; i++)
    {
        for (int d = 0; d < DIMENSIONS; d++)
        {
            if (currentDimension == d)
            {
                continue;
            }

            DataNode tmp = values[numNodes * d + i];

            if (tmp.cordinates[currentDimension] == branch->treeSplit)
            /*
            Potential replacement:
            { // cordinate might be left or right, need to check data and other cords     // could be added to either side, add to left if not full
                if (leftCounter[d] < rightMiddleIndex)
                {
                    memcpy(&leftDataNodes[d * rightMiddleIndex + leftCounter[d]++], &values[numNodes * d + i], sizeof(DataNode));
                }
                // add right
                else
                {
                    memcpy(&rightDataNodes[d * rightSize + rightCounter[d]++], &values[numNodes * d + i], sizeof(DataNode));
                }
            */
            { // cordinate might be left or right, need to check data and other cords
                bool goneLeft = false;
                for (int j = 0; j < rightMiddleIndex; j++)
                { // go through treeSplit left nodes, look for match
                    // TODO: ha en annen id også?
                    if (__cordEqual(tmp.cordinates, leftDataNodes[rightMiddleIndex * currentDimension + j].cordinates) && strcmp(tmp.location, leftDataNodes[rightMiddleIndex * currentDimension + j].location))
                    { // add value to left
                        goneLeft = true;
                        memcpy(&leftDataNodes[d * rightMiddleIndex + leftCounter[d]++], &values[numNodes * d + i], sizeof(DataNode));
                        break;
                    }
                }
                if (!goneLeft)
                { // right
                    memcpy(&rightDataNodes[d * rightSize + rightCounter[d]++], &values[numNodes * d + i], sizeof(DataNode));
                }
            }

            else if (tmp.cordinates[currentDimension] < branch->treeSplit)
            { // left
                memcpy(&leftDataNodes[d * rightMiddleIndex + leftCounter[d]++], &values[numNodes * d + i], sizeof(DataNode));
            }
            else
            { // right
                memcpy(&rightDataNodes[d * rightSize + rightCounter[d]++], &values[numNodes * d + i], sizeof(DataNode));
            }
        }
    }

    branch->left = KdbCreateBranch(leftDataNodes, rightMiddleIndex, root, nullptr, 1 % DIMENSIONS);
    branch->right = KdbCreateBranch(rightDataNodes, rightSize, root, nullptr, 1 % DIMENSIONS);

    // TODO: remove memory leak
    delete[] leftDataNodes;
    delete[] rightDataNodes;

    return branch;
}

KdbTree *KdbCreateTree(DataNode *values, int numNodes, long treeId)
{
    KdbTree *tree = new KdbTree;
    tree->size = numNodes;
    tree->id = treeId;
    printf("Tree|numNodes:%d\n", numNodes);

    int leftMiddleIndex = numNodes / 2 - !(numNodes % 2);
    int rightMiddleIndex = leftMiddleIndex + 1;

    int rightSize = rightMiddleIndex - (numNodes % 2);

    tree->treeSplit = (values[leftMiddleIndex].cordinates[0] + values[rightMiddleIndex].cordinates[0]) / 2;
    DataNode *leftDataNodes = new DataNode[rightMiddleIndex * DIMENSIONS];
    DataNode *rightDataNodes = new DataNode[rightSize * DIMENSIONS];

    // fill data
    memcpy(leftDataNodes, &values[0], sizeof(DataNode) * rightMiddleIndex);
    memcpy(rightDataNodes, &values[rightMiddleIndex], sizeof(DataNode) * rightSize);
    int leftCounter[DIMENSIONS] = {};
    int rightCounter[DIMENSIONS] = {};
    for (int i = 0; i < numNodes; i++)
    {
        for (int d = 1; d < DIMENSIONS; d++)
        {
            DataNode tmp = values[numNodes * d + i];

            if (tmp.cordinates[0] == tree->treeSplit)
            { // cordinate might be left or right, need to check data and other cords
                bool goneLeft = false;
                for (int j = 0; j < rightMiddleIndex; j++)
                { // go through treeSplit left nodes, look for match/*PIRAT datanode*/
                    if (__cordEqual(tmp.cordinates, leftDataNodes[j].cordinates) && strcmp(tmp.location, leftDataNodes[j].location))
                    { // add left
                        goneLeft = true;
                        memcpy(&leftDataNodes[d * rightMiddleIndex + leftCounter[d]++], &values[numNodes * d + i], sizeof(DataNode));
                        break;
                    }
                }
                if (!goneLeft)
                { // right
                    memcpy(&rightDataNodes[d * rightSize + rightCounter[d]++], &values[numNodes * d + i], sizeof(DataNode));
                }
            }

            else if (tmp.cordinates[0] < tree->treeSplit)
            { // left
                memcpy(&leftDataNodes[d * rightMiddleIndex + leftCounter[d]++], &values[numNodes * d + i], sizeof(DataNode));
            }
            else
            { // right
                memcpy(&rightDataNodes[d * rightSize + rightCounter[d]++], &values[numNodes * d + i], sizeof(DataNode));
            }
        }
    }
    tree->left = KdbCreateBranch(leftDataNodes, rightMiddleIndex, tree, nullptr, 1 % DIMENSIONS);
    tree->right = KdbCreateBranch(rightDataNodes, rightSize, tree, nullptr, 1 % DIMENSIONS);

    delete[] leftDataNodes;
    delete[] rightDataNodes;
    return tree;
}

void KdbDestroyTree(KdbTree *tree)
{
    if (tree == nullptr)
    {
        return;
    }
    // tree->leafList

    _KdbDestroyBranch(tree->left);
    _KdbDestroyBranch(tree->right);
    delete tree;
}

void _KdbDestroyBranch(KdbBranch *branch)
{
    if (branch->KdbLeafBuffer != nullptr)
        delete[] branch->KdbLeafBuffer;

    if (branch->left != nullptr)
        _KdbDestroyBranch(branch->left);
    if (branch->right != nullptr)
        _KdbDestroyBranch(branch->right);
    delete branch;
}