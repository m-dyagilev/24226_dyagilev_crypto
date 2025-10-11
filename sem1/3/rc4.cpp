#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>

constexpr int KEY_LEN = 8;
constexpr int S_LEN = 256;

enum ERRORS {
    NOT_ENOUGH_ARGS = 1,
    INVALID_PARAMETER,
    INCOMPATIBLE_KEY,
    FILE_NOT_OPENED,
    FILE_ACCESS_ERROR
};

static unsigned char getZ(std::array<unsigned char, S_LEN>& sBlock) {
    static unsigned char i = 0;
    static unsigned char j = 0;
    i++;
    j += sBlock[i];
    std::swap(sBlock[i], sBlock[j]);
    unsigned char t = sBlock[i] + sBlock[j];
    return sBlock[t];
}

static int applyCipher(std::ostream& os, std::istream& input,
                       std::array<unsigned char, S_LEN>& sBlock) {
    int result = 0;
    while (input.good()) {
        unsigned char in = input.get();
        if (input.eof()) {
            break;
        }
        if (!input.good() || !os.good()) {
            result = FILE_ACCESS_ERROR;
            break;
        }
        unsigned char z = getZ(sBlock);
        unsigned char res = (in ^ z);
        os << res;
    }
    return result;
}

static int readKeyFromFile(std::array<unsigned char, KEY_LEN>& key,
                           std::istream& in) {
    if (!in.good()) {
        return FILE_ACCESS_ERROR;
    }
    for (size_t i = 0; i < KEY_LEN; ++i) {
        key[i] = in.get();
        if (!in.good()) {
            return FILE_ACCESS_ERROR;
        }
    }
    return 0;
}

static void fillSBlock(std::array<unsigned char, S_LEN>& sBlock,
                       std::array<unsigned char, KEY_LEN>& key) {
    for (size_t i = 0; i < S_LEN; ++i) {
        sBlock[i] = i;
    }
    unsigned char j = 0;
    for (size_t i = 0; i < S_LEN; ++i) {
        j = j + sBlock[i] + key[i % KEY_LEN];
        std::swap(sBlock[i], sBlock[j]);
    }
}

int main(int argc, char** argv) {
    if (4 > argc) {
        std::cerr << "Need 3 arguments: key, input and output filenames"
                  << std::endl;
        return NOT_ENOUGH_ARGS;
    }
    std::filesystem::path keyPath(argv[1]);
    std::ifstream keyfile(keyPath, std::ios::binary | std::ios::in);
    if (!keyfile.is_open()) {
        std::cerr << "Cannot open key file " << argv[1] << std::endl;
        return FILE_NOT_OPENED;
    }
    std::filesystem::path inputPath(argv[2]);
    std::ifstream input(argv[2], std::ios::binary | std::ios::in);
    if (!input.is_open()) {
        std::cerr << "Cannot open input file " << argv[2] << std::endl;
        keyfile.close();
        return FILE_NOT_OPENED;
    }
    std::ofstream output(argv[3], std::ios::binary | std::ios::out);
    if (!output.is_open()) {
        std::cerr << "Cannot open output file " << argv[3] << std::endl;
        keyfile.close();
        input.close();
        return FILE_NOT_OPENED;
    }
    size_t keyLen = std::filesystem::file_size(keyPath);
    if (keyLen != KEY_LEN) {
        std::cerr << "Key must have length " << KEY_LEN << ", but has "
                  << keyLen << std::endl;
        keyfile.close();
        input.close();
        output.close();
        return INCOMPATIBLE_KEY;
    }

    std::array<unsigned char, KEY_LEN> key;
    if (readKeyFromFile(key, keyfile)) {
        std::cerr << "Failed reading key" << std::endl;
        keyfile.close();
        input.close();
        output.close();
        return FILE_ACCESS_ERROR;
    }
    keyfile.close();

    std::array<unsigned char, S_LEN> sBlock;
    fillSBlock(sBlock, key);

    int result = applyCipher(output, input, sBlock);

    switch (result) {
        case FILE_ACCESS_ERROR:
            std::cerr << "File access error" << std::endl;
            break;
    }
    keyfile.close();
    input.close();
    output.close();
    return result;
}
