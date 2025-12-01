#include "ExtEuc.hpp"

#include <stddef.h>

#include <iostream>

int getBareExtEucResult(long long a, long long b, ExtEucResult& result) {
    if ((0 > a) || (0 > b)) {
        return -1;
    }

    long long old_r = a;
    long long old_x = 1;
    long long old_y = 0;
    long long r = b;
    long long x = 0;
    long long y = 1;

    while (0 != r) {
        long long quotient = old_r / r;
        long long new_r = old_r - quotient * r;
        old_r = r;
        r = new_r;
        long long new_x = old_x - quotient * x;
        old_x = x;
        x = new_x;
        long long new_y = old_y - quotient * y;
        old_y = y;
        y = new_y;
        // std::cout << "<q, ri, si, ti>: <" << quotient 
        // << ", " << r << ", " << x << ", " << y << ">" << std::endl;
    }

    result.gcd = old_r;
    result.x = old_x;
    result.y = old_y;

    return 0;
}

int getInverseFromEucResult(ExtEucResult eucResult, long long mod,
                            size_t& result) {
    if (1 != eucResult.gcd) {
        return -1;
    }
    if (0 > eucResult.x) {
        result = eucResult.x + mod;
    } else {
        result = eucResult.x;
    }
    return 0;
}

int getInverse(long long inverting, long long mod, size_t& result) {
    ExtEucResult eucResult = {0, 0, 0};
    // std::cout << "Getting inverse of " << inverting << " by mod " << mod << std::endl;
    if (0 != getBareExtEucResult(inverting, mod, eucResult)) {
        return -1;
    }
    int status = getInverseFromEucResult(eucResult, mod, result);
    // std::cout << "It is " << result << std::endl;
    return status;
}

int getGCD(long long a, long long b, long long& result) {
    ExtEucResult eucResult = {0, 0, 0};
    if (0 != getBareExtEucResult(a, b, eucResult)) {
        return -1;
    }
    result = eucResult.gcd;
    return 0;
}