#ifndef CONFIG
#define CONFIG
// Config:
#define KDB_LEAF_SIZE 32

#define DIMENSIONS 2
#define CHARACTER_LIMIT 16

// if thread buffer is small, threads will complete fast, exit and queue up bulkloadings
// it will also increase the communication between threads as there would be more writes to global memory per insert
// however smaller buffers would cause readers to have more updated data.
// should perform a test testing the effect of different thread buffer sizes.
#define THREAD_BUFFER_SIZE 256
#define GLOBAL_B_CHUNK_SIZE 4
#define GLOBAL_BUFFER_SIZE (GLOBAL_B_CHUNK_SIZE * THREAD_BUFFER_SIZE)

// max (GLOBAL_BUFFER_SIZE * 2) * 5 size trees
#define MAX_BULKLOAD_LEVEL 8
#define SCHEDULER_MAP_ARRAY_SIZE 64

#define API_DELAY_MS 0

#define INITIAL_WRITERS 8
#define INITIAL_READERS 0

#endif