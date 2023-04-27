#ifndef CONFIG
#define CONFIG
// Config:

#define DIMENSIONS 2
#define CHARACTER_LIMIT 16

// if thread buffer is small, threads will complete fast, exit and queue up bulkloadings
// it will also increase the communication between threads as there would be more writes to global memory per insert
// however smaller buffers would cause readers to have more updated data.
// should perform a test testing the effect of different thread buffer sizes.
#define KDB_LEAF_SIZE 64

#define THREAD_BUFFER_SIZE 512
// Chunk size should be a size so that a bulkload is faster than filling the entire memory
#define GLOBAL_B_CHUNK_SIZE 8
#define GLOBAL_BUFFER_SIZE (GLOBAL_B_CHUNK_SIZE * THREAD_BUFFER_SIZE)

// max (GLOBAL_BUFFER_SIZE * 2) * 5 size trees
#define MAX_BULKLOAD_LEVEL 1
#define SCHEDULER_MAP_ARRAY_SIZE 1024

#define EPOCH_WAIT_NUM 10
#define LARGE_BULKLOAD_SIZE 2
#define LARGEST_BULKLOAD_CAP 2048

#define API_DELAY_MS 0
#define API_DATA_TYPE 1 // 0 -> uniform data, 1 -> normal distribution
#define API_MIN 10000   // min or mean
#define API_MAX 1000    // max or standard deviation

#define INITIAL_WRITERS 2
#define INITIAL_READERS 2
#define BULK_THREAD 1

#define BLOOM_SIZE 1000000
#define BLOOM_NUM_HASHES 6

#define TREES_CREATED 1000

#endif