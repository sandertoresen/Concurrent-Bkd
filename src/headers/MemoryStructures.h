#ifndef MEMORY_STRUCTURES_H
#define MEMORY_STRUCTURES_H
#include "Config.h"

using namespace std;

struct DataNode
{
    float cordinates[DIMENSIONS];
    char location[CHARACTER_LIMIT];
};

#endif