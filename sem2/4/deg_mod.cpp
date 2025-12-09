#include "deg_mod.hpp"

#include <iostream>

size_t powerOfTwoPowModEveryMult(size_t base, size_t power_of_two, size_t mod) {
    size_t result = base % mod;
    for (size_t i = 0; i < power_of_two; ++i) {
        result = result * result % mod;
    }
    return result;
}

size_t degMod(size_t arg, size_t deg, size_t mod) {
    if (0 == arg) {
        return 0;
    }
    if (0 == deg) {
        return 1;
    }
    size_t result = 1;
    unsigned int idx = 0;
    while (0 < deg) {
        // std::cout << "Degree bit " << idx << " is ";
        if (0 < deg % 2) {
            // std::cout << "1, current result is ";
            result = result * powerOfTwoPowModEveryMult(arg, idx, mod) % mod;
            // std::cout << result << std::endl;
        } else {
            // std::cout << "0" << std::endl;
        }
        deg >>= 1;
        idx++;
    }
    return result;
}
