#include <iostream>

#include "deg_mod.hpp"
#include "dhData.hpp"

enum ERRORS {
    NOT_ENOUGH_ARGS = 1,
    INVALID_PARAMETER,
    NO_G_FOUND,
    UNKNOWN_ERROR,
};

int runDH(size_t N, size_t q) {
    size_t p = 2 * q + 1;
    size_t g = 0;
    for (size_t i = 2; i < p - 1; ++i) {
        if (1 != degMod(i, q, p)) {
            g = i;
            break;
        }
    }
    if (0 == g) {
        return NO_G_FOUND;
    }
    DHData dhData(N, p, g);
    dhData.init();
    dhData.print(std::cout);
    bool finish = false;
    while (!finish) {
        std::cout << "Please, enter first abonent number or -1 to exit: ";
        long long choice = -1;
        std::cin >> choice;
        if (0 > choice) {
            return 0;
        }
        if (N <= (size_t)choice) {
            std::cout << "Invalid number, max is " << (N - 1) << std::endl;
            continue;
        }
        size_t idx1 = choice;
        std::cout << "Please, enter second abonent number or -1 to exit: ";
        choice = -1;
        std::cin >> choice;
        if (0 > choice) {
            return 0;
        }
        if (N <= (size_t)choice) {
            std::cout << "Invalid number, max is " << (N - 1) << std::endl;
            continue;
        }
        size_t idx2 = choice;
        if (idx1 != idx2) {
            dhData.generateCommonKey(idx1, idx2);
        } else {
            std::cout << "Equal idxs are chosen!" << std::endl;
        }
    }
    return UNKNOWN_ERROR;
}

int main(int argc, char** argv) {
    if (3 > argc) {
        std::cerr << "Need 2 arguments: N and a prime q such that p == 2q + 1"
                  << std::endl;
        return NOT_ENOUGH_ARGS;
    }
    long long N = std::atoll(argv[1]);
    if ((2 > N)) {
        std::cerr << "Invalid N, must be >=2" << std::endl;
        return INVALID_PARAMETER;
    }
    long long q = std::atoll(argv[2]);
    if ((2 > q)) {
        std::cerr << "Invalid q, must be a prime number >=2" << std::endl;
        return INVALID_PARAMETER;
    }
    int result = runDH(N, q);
    switch (result) {
        case NO_G_FOUND:
            std::cerr << "Could not find g" << std::endl;
            break;
        case UNKNOWN_ERROR:
            std::cerr << "Unexpected exit from loop" << std::endl;
            break;
    }
    return result;
}
