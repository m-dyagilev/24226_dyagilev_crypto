#include <iostream>
#include <fstream>
#include <random>
#include <ctime>

enum Modes {
    ALL_ONES,
    RANDOM_ONES_ZEROS,
    HALF_SPLIT,
    RANDOM,
    MAX_MODE
};

enum Errors {
    NOT_ENOUGH_ARGS = 1,
    INVALID_PARAMETER,
    FILE_NOT_OPENED,
    WRITE_ERROR,
};

int putNRandomOnesZeros(std::ostream& output, size_t n) {
    std::mt19937 generator;
    generator.seed(std::time(NULL));
    for (size_t i = 0; i < n; ++i) {
        if (!output.good()) {
            return WRITE_ERROR;
        }
        unsigned char nextChar = generator() % 2 ? '1' : '0';
        output << nextChar;
    }
    return 0;
}

int putNRandomChars(std::ostream& output, size_t n) {
    std::mt19937 generator;
    generator.seed(std::time(NULL));
    for (size_t i = 0; i < n; ++i) {
        if (!output.good()) {
            return WRITE_ERROR;
        }
        unsigned char nextChar = generator() % 256;
        output << nextChar;
    }
    return 0;
}

int putNChars(std::ostream& output, unsigned char c, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        if (!output.good()) {
            return WRITE_ERROR;
        }
        output << c;
    }
    return 0;
}

int putHalfSplitOnesRandoms(std::ostream& output, size_t n) {
    int lengthOnes = n / 2;
    int lengthRandoms = n - lengthOnes;
    int result = putNChars(output, '1', lengthOnes);
    if (result) {
        return result;
    }
    result = putNRandomChars(output, lengthRandoms);
    return result;
}

int main(int argc, char** argv) {
    constexpr size_t maxLength = 100000;
    if (4 > argc) {
        std::cerr << "Need 3 arguments: type of file to generate, " 
                  << "output filename and length from (0," << maxLength << ")\n" 
                  << "Possible modes:\n"
                  << "0 == all \'1\'s\n"
                  << "1 == random \'1\'s and \'0\'s\n"
                  << "2 == half \'1\'s, half randoms\n" 
                  << "3 == random chars from [0, 255]" << std::endl;
        return NOT_ENOUGH_ARGS;
    }
    int mode = std::atoi(argv[1]);
    if ((0 > mode) && (MAX_MODE <= mode)) {
        std::cerr << "Mode is invalid, must belong to " 
                  << "[0, " << MAX_MODE << "]" << std::endl;
        return INVALID_PARAMETER;
    }
    size_t length = std::atoi(argv[3]);
    if ((0 > length) || (maxLength < length)) {
        std::cerr << "Length is invalid, must belong to (0, " 
                  << maxLength << ")" << std::endl;
        return INVALID_PARAMETER;
    }
    std::ofstream output(argv[2]);
    if (!output.is_open()) {
        std::cerr << "Cannot open/create output file " << argv[2] << std::endl;
        return FILE_NOT_OPENED;
    }
    int result = 0;
    switch (mode) {
        case ALL_ONES:
            result = putNChars(output, '1', length);
            break;
        case RANDOM_ONES_ZEROS:
            result = putNRandomOnesZeros(output, length);
            break;
        case HALF_SPLIT:
            result = putHalfSplitOnesRandoms(output, length);
            break;
        case RANDOM:
            result = putNRandomChars(output, length);
            break;
    }
    switch (result) {
        case WRITE_ERROR:
            std::cerr << "Writing to file failed" << std::endl;
            break;
    }
    output.close();
    return result;
}
