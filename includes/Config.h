#ifndef CONFIG
#define CONFIG
// Config:
static const int KDB_LEAF_SIZE = 4;

static const int DIMENSIONS = 2;
static const int CHARACTER_LIMIT = 32;

static const int THREAD_BUFFER_SIZE = 16;
static const int GLOBAL_B_CHUNK_SIZE = 4;
static const int GLOBAL_BUFFER_SIZE = GLOBAL_B_CHUNK_SIZE * THREAD_BUFFER_SIZE;

#endif