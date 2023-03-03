#ifndef SCHEDULER
#define SCHEDULER

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



*/
#endif