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

static int listDecodes(std::ostream& output, std::istream& input) {
    constexpr unsigned minKey = 0;
    constexpr unsigned maxKey = 25;
    int result = 0;
    for (size_t i = minKey; i <= maxKey; ++i) {
        output << "\n\nKey == " << i << ": " << std::endl;
        input.clear();
        input.seekg(0);
        result = decode(output, input, i);
        if (result) {
            break;
        }
    }
    return result;
}

int main(int argc, char** argv) {
    if (2 > argc) {
        std::cerr << "Need 1 argument: input filename" << std::endl;
        return NOT_ENOUGH_ARGS;
    }
    std::ifstream input(argv[1]);
    if (!input.is_open()) {
        std::cerr << "Cannot open input file " << argv[1] << std::endl;
        return FILE_NOT_OPENED;
    }
    int result = listDecodes(std::cout, input);
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
