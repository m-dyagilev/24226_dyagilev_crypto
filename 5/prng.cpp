#include <array>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

enum ERRORS {
    NOT_ENOUGH_ARGS = 1,
    INCOMPATIBLE_KEY,
    FILE_NOT_OPENED,
    FILE_ACCESS_ERROR,
    INVALID_PARAMETER,
};

namespace TEA {
constexpr size_t KEY_LEN_BYTES = 16;
constexpr size_t BLOCK_LEN_BYTES = 8;

constexpr size_t DELTA = 0x9e3779b9;
constexpr size_t NUMBER_OF_ROUNDS = 32;

static void encrypt(uint32_t* text /*[2]*/, uint32_t* key /*[4]*/) {
    uint32_t y = text[0], z = text[1], sum = 0, n = NUMBER_OF_ROUNDS;
    while (0 < n--) {
        sum += DELTA;
        y += ((z << 4) + key[0]) ^ (z + sum) ^ ((z >> 5) + key[1]);
        z += ((y << 4) + key[2]) ^ (y + sum) ^ ((y >> 5) + key[3]);
    }
    text[0] = y;
    text[1] = z;
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

static uint64_t hash(const std::array<uint8_t, KEY_LEN_BYTES>& key,
                     uint8_t* input, size_t len) {
    uint64_t hash = 0;
    size_t curBlockStart = 0;
    uint8_t block[BLOCK_LEN_BYTES];
    while (len > curBlockStart) {
        size_t curBlockEnd = curBlockStart + BLOCK_LEN_BYTES;
        size_t toPad = (len >= curBlockEnd) ? 0 : (curBlockEnd - len);
        for (size_t i = 0; i < BLOCK_LEN_BYTES - toPad; ++i) {
            block[i] = input[curBlockStart + i];
        }
        for (size_t i = BLOCK_LEN_BYTES - toPad; i < BLOCK_LEN_BYTES; ++i) {
            block[i] = 0;
        }
        encrypt((uint32_t*)block, (uint32_t*)key.data());
        hash = *((uint64_t*)block) ^ hash;
        curBlockStart = curBlockEnd;
    }
    return hash;
}
};  // namespace TEA

namespace LCG {
static uint64_t next = 1;

void srand(uint64_t seed) { next = seed; }

uint64_t rand(void) {
    next = (next * 1103515245 + 12345) % 2147483648;
    return next;
}
};  // namespace LCG

std::vector<uint64_t> getNRandomNumbers(
    const std::array<uint8_t, TEA::KEY_LEN_BYTES>& key, size_t N) {
    std::vector<uint64_t> result(N);
    LCG::srand(std::time(NULL));
    for (size_t i = 0; i < N; ++i) {
        uint64_t curRand = LCG::rand();
        result[i] = TEA::hash(key, (uint8_t*)&curRand, 8);
    }
    return result;
}

int main(int argc, char** argv) {
    if (3 > argc) {
        std::cerr << "Need 2 arguments: path to file with TEA key and number "
                     "of numbers to be generated"
                  << std::endl;
        return NOT_ENOUGH_ARGS;
    }

    std::filesystem::path keyPath(argv[1]);
    size_t keyLen = std::filesystem::file_size(keyPath);
    if (keyLen != TEA::KEY_LEN_BYTES) {
        std::cerr << "Key must have length " << TEA::KEY_LEN_BYTES
                  << ", but has " << keyLen << std::endl;
        return INCOMPATIBLE_KEY;
    }

    int num = std::atoi(argv[2]);
    if (0 >= num) {
        std::cerr << "Number is invalid, must be more than 0" << std::endl;
        return INVALID_PARAMETER;
    }

    std::ifstream keyfile(keyPath, std::ios::binary | std::ios::in);
    if (!keyfile.is_open()) {
        std::cerr << "Cannot open key file " << argv[1] << std::endl;
        return FILE_NOT_OPENED;
    }
    std::array<uint8_t, TEA::KEY_LEN_BYTES> key;
    if (TEA::readKeyFromFile(key, keyfile)) {
        std::cerr << "Failed reading key" << std::endl;
        keyfile.close();
        return FILE_ACCESS_ERROR;
    }
    keyfile.close();

    std::vector<uint64_t> randomNubers = getNRandomNumbers(key, num);
    std::cout << num << " random numbers:" << std::endl;
    for (const uint64_t& number : randomNubers) {
        std::cout << number << " ";
    }
    std::cout << std::endl;
}
