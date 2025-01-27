#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cctype>

enum Errors {
    NOT_ENOUGH_ARGS = 1,
    INVALID_PARAMETER,
    FILE_NOT_OPENED,
    INVALID_INPUT,
    READING_ERROR,
    OUTPUT_ERROR,
};

static unsigned char shiftChar(unsigned char c, int shift) {
    unsigned int rawShifted = c + shift;
    unsigned char mod = 0;
    if (std::isupper(c)) {
        mod = rawShifted % ('Z' + 1);
        if ('A' > mod) {
            if (0 < shift) {
                return 'A' + mod;
            }
            else {
                return 'Z' - ('A' - mod - 1);
            }
        }
        else {
            return mod;
        }
    }
    else {
        mod = rawShifted % ('z' + 1);
        if ('a' > mod) {
            if (0 < shift) {
                return 'a' + mod;
            }
            else {
                return 'z' - ('a' - mod - 1);
            }
        }
        else {
            return mod;
        }
    }
}

static int encode(std::ostream& output, std::istream& input, int key) {
    unsigned char curChar = 0;
    while (!input.eof()) {
        curChar = input.get();
        if (input.eof()) {
            return 0;
        }
        if (input.fail()) {
            return READING_ERROR;
        }
        if (std::isalpha(curChar)) {
            output << shiftChar(curChar, key);
        }
        else if (std::isspace(curChar)) {
            output << curChar;
        }
        else {
            return INVALID_INPUT;
        }
    }
    return 0;
}

static int decode(std::ostream& output, std::istream& input, int key) {
    return encode(output, input, -key);
}

int main(int argc, char** argv) {
    if (4 > argc) {
        std::cerr << "Need 3 arguments: mode (0 - encode, or 1 - decode)," <<
                     " key and input filename" << std::endl;
        return NOT_ENOUGH_ARGS;
    }
    int mode = std::atoi(argv[1]);
    if ((0 != mode) && (1 != mode)) {
        std::cerr << "Mode is invalid, must be either 0 or 1" << std::endl;
        return INVALID_PARAMETER;
    }
    int key = std::atoi(argv[2]);
    if ((0 > key) || (25 < key)) {
        std::cerr << "Key is invalid, must belong to [0, 25]" << std::endl;
        return INVALID_PARAMETER;
    }
    std::ifstream input(argv[3]);
    if (!input.is_open()) {
        std::cerr << "Cannot open input file " << argv[3] << std::endl;
        return FILE_NOT_OPENED;
    }
    int result = 0;
    if (0 == mode) {
        result = encode(std::cout, input, key);
    }
    else {
        result = decode(std::cout, input, key);
    }
    switch (result) {
        case INVALID_INPUT:
            std::cerr << "Input contains invalid characters: " <<
                         "only A-Z and a-z symbols are supported" << std::endl;
            break;
        case READING_ERROR:
            std::cerr << "Reading from file failed" << std::endl;
            break;
        case OUTPUT_ERROR:
            std::cerr << "Output to a specified stream failed" << std::endl;
            break;
        default:
            std::cout << std::endl;
    }
    input.close();
    return result;
}
