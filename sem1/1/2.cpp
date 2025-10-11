#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cctype>
#include <cassert>

enum Errors {
    NOT_ENOUGH_ARGS = 1,
    FILE_NOT_OPENED,
    INVALID_INPUT,
    READING_ERROR,
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

static int findKey(unsigned* key, std::istream& enc, std::istream& dec) {
    assert(nullptr != key);
    constexpr unsigned minKey = 0;
    constexpr unsigned maxKey = 25;
    unsigned char firstAlphaEnc = 0;
    unsigned char firstAlphaDec = 0;
    while (enc.good() && !std::isalpha(firstAlphaEnc)) {
        firstAlphaEnc = enc.get();
    }
    if (enc.fail()) {
        return READING_ERROR;
    }
    if (!std::isalpha(firstAlphaEnc)) {
        return INVALID_INPUT;
    }
    while (dec.good() && !std::isalpha(firstAlphaDec)) {
        firstAlphaDec = dec.get();
    }
    if (dec.fail()) {
        return READING_ERROR;
    }
    if (!std::isalpha(firstAlphaDec)) {
        return INVALID_INPUT;
    }
    for (size_t i = minKey; i <= maxKey; ++i) {
        if (firstAlphaEnc == shiftChar(firstAlphaDec, i)) {
            *key = i;
            return 0;
        }
    }
    return INVALID_INPUT;
}

int main(int argc, char** argv) {
    if (3 > argc) {
        std::cerr << "Need 2 arguments: encoded and decoded input filenames" << std::endl;
        return NOT_ENOUGH_ARGS;
    }
    std::ifstream inputEnc(argv[1]);
    if (!inputEnc.is_open()) {
        std::cerr << "Cannot open encoded input file " << argv[1] << std::endl;
        return FILE_NOT_OPENED;
    }
    std::ifstream inputDec(argv[2]);
    if (!inputDec.is_open()) {
        std::cerr << "Cannot open decoded input file " << argv[2] << std::endl;
        inputEnc.close();
        return FILE_NOT_OPENED;
    }
    unsigned key = 0;
    int result = findKey(&key, inputEnc, inputDec);
    switch (result) {
        case INVALID_INPUT:
            std::cerr << "Input is invalid: it contains invalid characters, " <<
                         "shift is not from [0, 25], or encoded input is not " << 
                         "really encoded from decoded input" << std::endl;
            break;
        case READING_ERROR:
            std::cerr << "Reading from file failed" << std::endl;
            break;
        case 0:
            std::cout << "Key is " << key << std::endl;
    }
    inputEnc.close();
    inputDec.close();
    return result;
}
