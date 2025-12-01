#include <iomanip>
#include <iostream>

enum ERRORS {
    NOT_ENOUGH_ARGS = 1,
    INVALID_PARAMETER,
};

void demonstrateLaw(size_t p) {
    const size_t WIDTH = 4;
    std::cout << std::setw(WIDTH) << "*";
    std::cout << "|";
    for (size_t i = 1; i < p; ++i) {
        std::cout << std::setw(WIDTH) << i;
    }
    std::cout << std::endl;
    for (size_t i = 1; i < p * WIDTH + 1; ++i) {
        std::cout << "-";
    }
    std::cout << std::endl;
    for (size_t i = 1; i < p; ++i) {
        std::cout << std::setw(WIDTH) << i;
        std::cout << "|";
        for (size_t j = 1; j < p; ++j) {
            std::cout << std::setw(WIDTH) << ((i * j) % p);
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
    demonstrateLaw(p);
}
