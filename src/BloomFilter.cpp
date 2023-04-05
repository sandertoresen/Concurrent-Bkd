#include <bitset>
#include <functional>
#include <iostream>
#include <cstring>
#include <pthread.h>
#include "headers/Config.h"
#include "headers/BloomFilter.h"

using std::bitset;

BloomFilter::BloomFilter(int size, int numHash)
{
    filter = bitset<100000>(size);
    numHashes = numHash;
}

BloomFilter::~BloomFilter()
{
    pthread_rwlock_destroy(&lock);
}

void BloomFilter::add(const char *value)
{
    for (int i = 0; i < numHashes; ++i)
    {
        char valueWithHash[CHARACTER_LIMIT + 32];
        std::strcpy(valueWithHash, value);
        std::strcat(valueWithHash, std::to_string(i).c_str());
        int currentHash = hashFunction(valueWithHash);
        int index = currentHash % filter.size();
        pthread_rwlock_wrlock(&lock);
        filter.set(index, true);
        pthread_rwlock_unlock(&lock);
    }
}

bool BloomFilter::contains(const char *value)
{
    for (int i = 0; i < numHashes; ++i)
    {
        char valueWithHash[CHARACTER_LIMIT + 32];
        std::strcpy(valueWithHash, value);
        std::strcat(valueWithHash, std::to_string(i).c_str());
        int currentHash = hashFunction(valueWithHash);
        int index = currentHash % filter.size();

        pthread_rwlock_rdlock(&lock);
        bool contain = (bool)!filter.test(index);
        pthread_rwlock_unlock(&lock);
        if (contain)
            return contain;
    }
    return false;
}
