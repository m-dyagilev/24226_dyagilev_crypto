#include <cassert>
#include <iostream>

enum ERRORS {
    NOT_ENOUGH_ARGS = 1,
    INVALID_PARAMETER,
};

static size_t powerOfTwoPowModEveryMult(size_t base, size_t power_of_two,
                                        size_t mod) {
    size_t result = base % mod;
    for (size_t i = 0; i < power_of_two; ++i) {
        result = result * result % mod;
    }
    return result;
}

static size_t degMod(size_t arg, size_t deg, size_t mod) {
    if (0 == arg) {
        return 0;
    }
    if (0 == deg) {
        return 1;
    }
    size_t result = 1;
    unsigned int idx = 0;
    while (0 < deg) {
        std::cout << "Degree bit " << idx << " is ";
        if (0 < deg % 2) {
            std::cout << "1, current result is ";
            result = result * powerOfTwoPowModEveryMult(arg, idx, mod) % mod;
            std::cout << result << std::endl;
        } else {
            std::cout << "0" << std::endl;
        }
        deg >>= 1;
        idx++;
    }
    return result;
}

int main(int argc, char* argv[]) {
    if (4 > argc) {
        std::cerr << "Need 3 arguments to perform a^b mod c: a, b and c"
                  << std::endl;
        return NOT_ENOUGH_ARGS;
    }
    int a = std::atoi(argv[1]);
    if (0 > a) {
        std::cerr << "Number a is invalid, must be not less than 0"
                  << std::endl;
        return INVALID_PARAMETER;
    }
    int b = std::atoi(argv[2]);
    if (0 > b) {
        std::cerr << "Number b is invalid, must be not less than 0"
                  << std::endl;
        return INVALID_PARAMETER;
    }
    if (0 == a && 0 == b) {
        std::cerr << "Numbers a and b cannot be zeros together" << std::endl;
        return INVALID_PARAMETER;
    }
    int c = std::atoi(argv[3]);
    if (0 >= c) {
        std::cerr << "Number c is invalid, must be more than 0" << std::endl;
        return INVALID_PARAMETER;
    }
    size_t opResult = degMod(a, b, c);
    std::cout << a << "^" << b << " mod " << c << " == " << opResult
              << std::endl;
}
