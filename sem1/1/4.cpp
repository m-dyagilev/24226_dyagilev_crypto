#include <set>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cctype>

struct Dictionary {
    Dictionary() {
        std::ifstream input("dictionary.txt");
        for (std::string line; std::getline(input, line);) {
            _words.insert(line);
        }
    }

    bool contains(const std::string& word) const {
        return _words.count(word);
    }
  
    std::set<std::string> _words;
};

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

static bool checkDecodedInDictionary(std::string& match, 
                                     Dictionary& dict, 
                                     std::istream& dec) {
    std::string token;
    while (dec.good()) {
        token.clear();
        dec >> token;
        if (dec.eof() && token.empty()) {
            break;
        }
        if (dec.fail()) {
            return false;
        }
        std::transform(token.begin(), token.end(), token.begin(), 
                       [](char c) { return std::tolower(c); });
        if (dict.contains(token)) {
            match = token;
            return true;
        }
    }
    return false;
}

static int findKey(unsigned* key, std::ostream& output, std::istream& input) {
    constexpr unsigned minKey = 0;
    constexpr unsigned maxKey = 25;
    int result = 0;
    std::string match;
    Dictionary dict;
    std::stringstream ss;
    for (size_t i = minKey; i <= maxKey; ++i) {
        input.clear();
        input.seekg(0);
        ss.str("");
        ss.clear();
        result = decode(ss, input, i);
        if (result) {
            break;
        }
        if (checkDecodedInDictionary(match, dict, ss)) {
            output << "\nWord " << match << " found for key " << i << std::endl;
            output << "Decoded text: " << std::endl;
            output << ss.str() << std::endl;
            *key = i;
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
    unsigned key = 0;
    int result = findKey(&key, std::cout, input);
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
