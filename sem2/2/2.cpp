#include <algorithm>
#include <iomanip>
#include <iostream>
#include <vector>

enum ERRORS {
    NOT_ENOUGH_ARGS = 1,
    INVALID_PARAMETER,
};

void demonstrateOrders(size_t p) {
    const size_t WIDTH = 6;
    std::cout << std::left;
    std::cout << std::setw(WIDTH) << "a" << "|";
    for (size_t i = 1; i < p; ++i) {
        std::cout << "a^" << std::setw(WIDTH - 2) << i;
    }
    std::cout << "|" << std::endl;
    for (size_t i = 1; i < p * WIDTH + 2; ++i) {
        std::cout << "-";
    }
    std::cout << "|" << std::endl;
    for (size_t i = 1; i < p; ++i) {
        std::vector<bool> values(p, false);
        size_t curValue = i;
        values[curValue] = true;
        std::cout << std::setw(WIDTH) << i << "|" << std::setw(WIDTH) << i;
        bool calc = true;
        for (size_t j = 2; j < p; ++j) {
            if (calc) {
                curValue = (curValue * i) % p;
                if (values[curValue]) {
                    calc = false;
                    for (size_t k = 0; k < WIDTH; ++k) {
                        std::cout << " ";
                    }
                } else {
                    values[curValue] = true;
                    std::cout << std::setw(WIDTH) << curValue;
                }
            } else {
                for (size_t k = 0; k < WIDTH; ++k) {
                    std::cout << " ";
                }
            }
        }
        size_t count = std::count(values.begin(), values.end(), true);
        std::cout << "|" << count;
        if (p - 1 == count) {
            std::cout << " - generator!";
        }
        std::cout << std::endl;
    }
}

int main(int argc, char** argv) {
    if (2 > argc) {
        std::cerr << "Need 1 argument: P" << std::endl;
        return NOT_ENOUGH_ARGS;
    }
    long long p = std::atoll(argv[1]);
    if ((2 >= p)) {
        std::cerr << "Invalid P, must be a prime number >2" << std::endl;
        return INVALID_PARAMETER;
    }
    demonstrateOrders(p);
}