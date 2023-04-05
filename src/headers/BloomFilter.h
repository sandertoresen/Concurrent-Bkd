#ifndef BLOOMFILTER_H
#define BLOOMFILTER_H

#include <bitset>
#include <functional>
#include <pthread.h>

using std::bitset;

class BloomFilter
{
public:
    BloomFilter(int size, int numHash);
    ~BloomFilter();
    void add(const char *value);

    bool contains(const char *value);

private:
    bitset<100000> filter;
    int numHashes;
    std::hash<char *> hashFunction;
    pthread_rwlock_t lock = PTHREAD_RWLOCK_INITIALIZER;
};

#endif