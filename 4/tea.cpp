#include <array>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>

enum ERRPOS {
    NOT_ENOUGH_ARGS = 1,
    INVALID_PARAMETER,
    FILE_NOT_OPENED,
    FILE_ACCESS_ERROR,
    INCOMPATIBLE_KEY,
};

enum class Mode {
    ENCRYPT = 0,  // Generate nonce
    DECRYPT = 1,  // Read nonce as first block
};

#define BLOCK_LEN_BYTES 8
#define KEY_LEN_BYTES 16

#define DELTA 0x9e3779b9
#define NUMBER_OF_ROUNDS 32

void encrypt(uint32_t* text /*[2]*/, uint32_t* key /*[4]*/) {
    uint32_t y = text[0], z = text[1], sum = 0, n = NUMBER_OF_ROUNDS;
    while (0 < n--) {
        sum += DELTA;
        y += ((z << 4) + key[0]) ^ (z + sum) ^ ((z >> 5) + key[1]);
        z += ((y << 4) + key[2]) ^ (y + sum) ^ ((y >> 5) + key[3]);
    }
    text[0] = y;
    text[1] = z;
}

void decrypt(uint32_t* text /*[2]*/, uint32_t* key /*[4]*/) {
    uint32_t y = text[0], z = text[1], sum = DELTA << 5, n = NUMBER_OF_ROUNDS;
    while (0 < n--) {
        z -= ((y << 4) + key[2]) ^ (y + sum) ^ ((y >> 5) + key[3]);
        y -= ((z << 4) + key[0]) ^ (z + sum) ^ ((z >> 5) + key[1]);
        sum -= DELTA;
    }
    text[0] = y;
    text[1] = z;
}

static int tryGet8BytesBlock(std::array<uint8_t, BLOCK_LEN_BYTES>& block,
                             size_t& bytesRead, std::istream& input) {
    int result = 0;
    bytesRead = 0;
    if (!input.good()) {
        return FILE_ACCESS_ERROR;
    }
    while (BLOCK_LEN_BYTES > bytesRead) {
        uint8_t in = input.get();
        if (input.eof()) {
            break;
        }
        if (!input.good()) {
            result = FILE_ACCESS_ERROR;
            break;
        }
        block[bytesRead] = in;
        bytesRead++;
    }
    return result;
}

// Output feedback mode
static int applyCipher(std::ostream& os, std::istream& input,
                       const std::array<uint8_t, KEY_LEN_BYTES>& key,
                       Mode mode) {
    if (!os.good()) {
        return FILE_ACCESS_ERROR;
    }
    uint64_t nonce = 0;
    uint64_t encryptedNonce = nonce;
    std::mt19937_64 prng(time(nullptr));
    switch (mode) {
        case Mode::ENCRYPT:
            nonce = prng();
            encryptedNonce = nonce;
            encrypt((uint32_t*)(&encryptedNonce), (uint32_t*)key.data());
            os.write((char*)&encryptedNonce, BLOCK_LEN_BYTES);
            if (!os.good()) {
                return FILE_ACCESS_ERROR;
            }
            break;
        case Mode::DECRYPT:
            std::array<uint8_t, BLOCK_LEN_BYTES> nonceBlock;
            size_t gotBytes = 0;
            int readingResult = tryGet8BytesBlock(nonceBlock, gotBytes, input);
            if (0 != readingResult || (BLOCK_LEN_BYTES != gotBytes)) {
                return FILE_ACCESS_ERROR;
            }
            nonce = *((uint64_t*)nonceBlock.data());
            decrypt((uint32_t*)(&nonce), (uint32_t*)key.data());
            break;
    }
    std::array<uint8_t, BLOCK_LEN_BYTES> block;
    size_t bytesInBlock = 0;
    uint64_t z = nonce;
    while (input.good()) {
        int readingResult = tryGet8BytesBlock(block, bytesInBlock, input);
        if (0 != readingResult ||
            (BLOCK_LEN_BYTES != bytesInBlock && !input.eof())) {
            return FILE_ACCESS_ERROR;
        }
        encrypt((uint32_t*)(&z), (uint32_t*)key.data());
        uint64_t c = *(uint64_t*)block.data() ^ z;
        os.write((char*)&c, bytesInBlock);
        if (!os.good()) {
            return FILE_ACCESS_ERROR;
        }
        if (input.eof()) {
            break;
        }
    }
    return 0;
}

static int readKeyFromFile(std::array<uint8_t, KEY_LEN_BYTES>& key,
                           std::istream& in) {
    if (!in.good()) {
        return FILE_ACCESS_ERROR;
    }
    for (size_t i = 0; i < KEY_LEN_BYTES; ++i) {
        key[i] = in.get();
        if (!in.good()) {
            return FILE_ACCESS_ERROR;
        }
    }
    return 0;
}

int main(int argc, char** argv) {
    if (5 > argc) {
        std::cerr << "Need 4 arguments: mode (0 - encode, or 1 - decode),"
                  << " key, input and output filenames" << std::endl;
        return NOT_ENOUGH_ARGS;
    }
    int modeNum = std::atoi(argv[1]);
    if ((0 != modeNum) && (1 != modeNum)) {
        std::cerr << "Mode is invalid, must be either 0 or 1" << std::endl;
        return INVALID_PARAMETER;
    }
    Mode mode = static_cast<Mode>(modeNum);
    std::filesystem::path keyPath(argv[2]);
    std::ifstream keyfile(keyPath, std::ios::binary | std::ios::in);
    if (!keyfile.is_open()) {
        std::cerr << "Cannot open key file " << argv[1] << std::endl;
        return FILE_NOT_OPENED;
    }
    std::filesystem::path inputPath(argv[3]);
    std::ifstream input(argv[3], std::ios::binary | std::ios::in);
    if (!input.is_open()) {
        std::cerr << "Cannot open input file " << argv[2] << std::endl;
        keyfile.close();
        return FILE_NOT_OPENED;
    }
    std::ofstream output(argv[4], std::ios::binary | std::ios::out);
    if (!output.is_open()) {
        std::cerr << "Cannot open output file " << argv[3] << std::endl;
        keyfile.close();
        input.close();
        return FILE_NOT_OPENED;
    }
    size_t keyLen = std::filesystem::file_size(keyPath);
    if (keyLen != KEY_LEN_BYTES) {
        std::cerr << "Key must have length " << KEY_LEN_BYTES << ", but has "
                  << keyLen << std::endl;
        keyfile.close();
        input.close();
        output.close();
        return INCOMPATIBLE_KEY;
    }

    std::array<uint8_t, KEY_LEN_BYTES> key;
    if (readKeyFromFile(key, keyfile)) {
        std::cerr << "Failed reading key" << std::endl;
        keyfile.close();
        input.close();
        output.close();
        return FILE_ACCESS_ERROR;
    }
    keyfile.close();

    int result = applyCipher(output, input, key, mode);

    switch (result) {
        case FILE_ACCESS_ERROR:
            std::cerr << "File access error" << std::endl;
            break;
    }
    input.close();
    output.close();
    return result;
}