#ifndef BLOOMFILTER_H
#define BLOOMFILTER_H

#include <bitset>
#include <functional>

using std::bitset;

class BloomFilter
{
public:
    BloomFilter(int size, int numHash);
    void add(const char *value);

    bool contains(const char *value);

private:
    bitset<100000> filter;
    int numHashes;
    std::hash<char *> hashFunction;
};

#endif