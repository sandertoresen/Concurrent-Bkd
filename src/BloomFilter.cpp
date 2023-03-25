#include <bitset>
#include <functional>
#include <iostream>
#include <cstring>
#include "headers/Config.h"
#include "headers/BloomFilter.h"

using std::bitset;

BloomFilter::BloomFilter(int size, int numHash)
{
    filter = bitset<100000>(size);
    numHashes = numHash;
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
        filter.set(index, true);
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
        if (!filter.test(index))
        {
            return false;
        }
    }
    return true;
}
