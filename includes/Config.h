#ifndef CONFIG
#define CONFIG
// Config:
static const int KDB_LEAF_SIZE = 4;

static const int DIMENSIONS = 2;
static const int CHARACTER_LIMIT = 32;

// if thread buffer is small, threads will complete fast, exit and queue up bulkloadings
// it will also increase the communication between threads as there would be more writes to global memory per insert
// however smaller buffers would cause readers to have more updated data.
// should perform a test testing the effect of different thread buffer sizes.
static const int THREAD_BUFFER_SIZE = 128;
static const int GLOBAL_B_CHUNK_SIZE = 4;
static const int GLOBAL_BUFFER_SIZE = GLOBAL_B_CHUNK_SIZE * THREAD_BUFFER_SIZE;

// max (GLOBAL_BUFFER_SIZE * 2) * 5 size trees
static const int MAX_BULKLOAD_LEVEL = 5;

#endif