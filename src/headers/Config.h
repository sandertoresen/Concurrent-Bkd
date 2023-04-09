#ifndef CONFIG
#define CONFIG
// Config:

#define DIMENSIONS 2
#define CHARACTER_LIMIT 16

// if thread buffer is small, threads will complete fast, exit and queue up bulkloadings
// it will also increase the communication between threads as there would be more writes to global memory per insert
// however smaller buffers would cause readers to have more updated data.
// should perform a test testing the effect of different thread buffer sizes.
#define KDB_LEAF_SIZE 128

#define THREAD_BUFFER_SIZE 1024
#define GLOBAL_B_CHUNK_SIZE 8
#define GLOBAL_BUFFER_SIZE (GLOBAL_B_CHUNK_SIZE * THREAD_BUFFER_SIZE)

// max (GLOBAL_BUFFER_SIZE * 2) * 5 size trees
#define MAX_BULKLOAD_LEVEL 4
#define SCHEDULER_MAP_ARRAY_SIZE 32

#define API_DELAY_MS 0

#define INITIAL_WRITERS 2
#define INITIAL_READERS 1
#define BULK_THREAD 1

#define BLOOM_SIZE 1000000
#define BLOOM_NUM_HASHES 6

#define TREES_CREATED 10000

#endif