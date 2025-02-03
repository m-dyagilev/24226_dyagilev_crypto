#include <iostream>
#include <fstream>
#include <map>
#include <cmath>

enum Errors {
    NOT_ENOUGH_ARGS = 1,
    FILE_NOT_OPENED,
    READING_ERROR,
    EMPTY_MAP
};

int calcFrequencies(std::istream& input, 
                    std::map<unsigned char, size_t>& freqMap, 
                    size_t* symbolsTotal) {
    unsigned char curChar = 0;
    while (!input.eof()) {
        curChar = input.get();
        if (input.eof()) {
            return 0;
        }
        if (input.fail()) {
            return READING_ERROR;
        }
        freqMap[curChar]++;
        (*symbolsTotal)++;
    }
    return 0;
}

double calcEntropy(const std::map<unsigned char, size_t>& freqMap, 
                   size_t symbolsTotal) {
    size_t numberOfSymbols = symbolsTotal;
    double result = 0;
    for (const auto& [symbol, frequency] : freqMap) {
        double prob = static_cast<double>(frequency) / numberOfSymbols;
        result -= prob * std::log2(prob);
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
    std::map<unsigned char, size_t> freqMap;
    size_t symbolsTotal = 0;
    int result = calcFrequencies(input, freqMap, &symbolsTotal);
    input.close();
    if (result) {
        std::cerr << "Reading from file failed" << std::endl;
        return result;
    }
    if (!symbolsTotal) {
        std::cerr << "No symbols found" << std::endl;
        return EMPTY_MAP;
    }
    double entropy = calcEntropy(freqMap, symbolsTotal);
    std::cout << "Entropy is " << entropy << std::endl;
    return result;
}
