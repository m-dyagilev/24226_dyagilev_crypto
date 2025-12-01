#ifndef EXT_EUC_H
#define EXT_EUC_H

#include <stddef.h>

// ax + by = gcd(a, b)

struct ExtEucResult {
    long long gcd;
    long long x;
    long long y;
};

int getBareExtEucResult(long long a, long long b, ExtEucResult& result);

int getInverseFromEucResult(ExtEucResult eucResult, long long mod,
                            size_t& result);

int getInverse(long long inverting, long long mod, size_t& result);

int getGCD(long long a, long long b, long long& result);

#endif