#include <filesystem>
#include <fstream>
#include <iostream>

enum ERRORS {
    NOT_ENOUGH_ARGS = 1,
    INVALID_PARAMETER,
    FILE_NOT_OPENED,
    INCOMPATIBLE_KEY,
    FILE_ACCESS_ERROR
};

static int applyCipher(std::ostream& os, std::istream& input,
                       std::istream& key) {
    int result = 0;
    while (input.good()) {
        unsigned char in = input.get();
        unsigned char k = key.get();
        if (input.eof()) {
            break;
        }
        if (!input.good() || !key.good() || !os.good()) {
            result = FILE_ACCESS_ERROR;
            break;
        }
        unsigned char res = (in ^ k);
        os << res;
    }
    return result;
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
    size_t keySize = std::filesystem::file_size(keyPath);
    size_t inputSize = std::filesystem::file_size(inputPath);
    if (keySize != inputSize) {
        std::cerr << "Key and input sizes do not match: " << keySize << " and "
                  << inputSize << std::endl;
        keyfile.close();
        input.close();
        output.close();
        return INCOMPATIBLE_KEY;
    }

    int result = applyCipher(output, input, keyfile);

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
