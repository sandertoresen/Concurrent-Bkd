#include <iostream>
#include "includes/Config.h"
#include "includes/BkdTree.h"

int main()
{
    printf("Hello world! %d\n", CONFIG_TEST_VALUE);

    BkdTree *tree = new BkdTree;

    // destructors is automatically called when delete is called
    delete tree;
}

// TODO sjekk ut korrekt construksjon og destruksjon av klasser
