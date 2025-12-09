#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "libs/cryptoPP/files.h"
#include "libs/cryptoPP/filters.h"
#include "libs/cryptoPP/hex.h"
#include "libs/cryptoPP/hmac.h"
#include "libs/cryptoPP/osrng.h"
#include "libs/cryptoPP/sha.h"
#include "libs/json.hpp"

using json = nlohmann::json;

enum ERRORS {
    NOT_ENOUGH_ARGS = 1,
    INVALID_PARAMETER,
    FILE_NOT_OPENED,
    INCOMPATIBLE_KEY,
    FILE_ACCESS_ERROR,
    HARD_FAIL,
    INPUT_DATA_FORMAT_ERROR,
    VALIDATION_RESULT_NEGATIVE,
};

namespace ConstParams {
constexpr size_t KEY_LEN_BYTES = 16;
constexpr size_t NONCE_LEN_BYTES = 8;
};  // namespace ConstParams

static int readKeyFromFile(std::array<uint8_t, ConstParams::KEY_LEN_BYTES>& key,
                           std::istream& in) {
    if (!in.good()) {
        return FILE_ACCESS_ERROR;
    }
    for (size_t i = 0; i < ConstParams::KEY_LEN_BYTES; ++i) {
        key[i] = in.get();
        if (!in.good()) {
            return FILE_ACCESS_ERROR;
        }
    }
    return 0;
}

int validateMACs(std::istream& iInput, const std::filesystem::path& iDirPath,
                 const std::array<uint8_t, ConstParams::KEY_LEN_BYTES>& iKey) {
    bool negativeResultsFound = false;
    json jsonInput = json::parse(iInput);
    if (!jsonInput.is_array()) {
        return INPUT_DATA_FORMAT_ERROR;
    }
    for (auto& dataEntry : jsonInput) {
        if (!dataEntry.is_object()) {
            return INPUT_DATA_FORMAT_ERROR;
        }
        if (!dataEntry.contains("path") || !dataEntry.contains("nonce") ||
            !dataEntry.contains("mac")) {
            return INPUT_DATA_FORMAT_ERROR;
        }

        std::string curPathStr = dataEntry["path"].get<std::string>();
        std::filesystem::path curPath(iDirPath);
        curPath /= curPathStr;

        std::string nonceHexStr = dataEntry["nonce"].get<std::string>();
        std::array<uint8_t, ConstParams::NONCE_LEN_BYTES> nonce;
        CryptoPP::StringSource ssForNonce(
            nonceHexStr, true,
            new CryptoPP::HexDecoder(
                new CryptoPP::ArraySink(nonce.data(), nonce.size())));

        std::string macHexStr = dataEntry["mac"].get<std::string>();
        std::array<uint8_t, CryptoPP::HMAC<CryptoPP::SHA256>::DIGESTSIZE> mac;
        CryptoPP::StringSource ssForMAC(
            macHexStr, true,
            new CryptoPP::HexDecoder(
                new CryptoPP::ArraySink(mac.data(), mac.size())));

        std::ifstream curFile(curPath);
        if (!curFile.is_open()) {
            return FILE_ACCESS_ERROR;
        }

        std::vector<uint8_t> rawData;
        rawData.reserve(nonce.size() + std::filesystem::file_size(curPath) +
                        mac.size());
        for (uint8_t byte : nonce) {
            rawData.push_back(byte);
        }
        while (curFile.good()) {
            uint8_t byte;
            curFile >> byte;
            rawData.push_back(byte);
        }
        curFile.close();
        for (uint8_t byte : mac) {
            rawData.push_back(byte);
        }

        CryptoPP::HMAC<CryptoPP::SHA256> hmac(iKey.data(), iKey.size());
        const int flags =
            CryptoPP::HashVerificationFilter::Flags::HASH_AT_END |
            CryptoPP::HashVerificationFilter::Flags::THROW_EXCEPTION;
        try {
            CryptoPP::VectorSource(
                rawData, true,
                new CryptoPP::HashVerificationFilter(hmac, nullptr, flags));
        } catch (
            const CryptoPP::HashVerificationFilter::HashVerificationFailed& e) {
            negativeResultsFound = true;
            std::cout << "Negative result of verification for " << curPathStr
                      << std::endl;
        }
    }
    if (negativeResultsFound) {
        return VALIDATION_RESULT_NEGATIVE;
    }
    return 0;
}

int main(int argc, char** argv) {
    if (4 > argc) {
        std::cerr << "Need 3 arguments: "
                  << "path to directory that it is needed to process, "
                  << "key and mac data filenames" << std::endl;
        return NOT_ENOUGH_ARGS;
    }
    std::filesystem::path iDirPath(argv[1]);
    std::filesystem::path keyPath(argv[2]);
    std::ifstream keyfile(keyPath, std::ios::binary | std::ios::in);
    if (!keyfile.is_open()) {
        std::cerr << "Cannot open key file " << argv[2] << std::endl;
        return FILE_NOT_OPENED;
    }
    std::ifstream input(argv[3]);
    if (!input.is_open()) {
        std::cerr << "Cannot open input file " << argv[3] << std::endl;
        keyfile.close();
        return FILE_NOT_OPENED;
    }
    size_t keyLen = std::filesystem::file_size(keyPath);
    if (keyLen != ConstParams::KEY_LEN_BYTES) {
        std::cerr << "Key must have length " << ConstParams::KEY_LEN_BYTES
                  << ", but has " << keyLen << std::endl;
        keyfile.close();
        input.close();
        return INCOMPATIBLE_KEY;
    }

    std::array<uint8_t, ConstParams::KEY_LEN_BYTES> key;
    if (readKeyFromFile(key, keyfile)) {
        std::cerr << "Failed reading key" << std::endl;
        keyfile.close();
        input.close();
        return FILE_ACCESS_ERROR;
    }
    keyfile.close();

    try {
        int result = validateMACs(input, iDirPath, key);
        switch (result) {
            case FILE_ACCESS_ERROR:
                std::cerr << "File access error" << std::endl;
                break;
            case VALIDATION_RESULT_NEGATIVE:
                std::cerr << "MAC validation result is negative for some files!"
                          << std::endl;
                break;
            case INPUT_DATA_FORMAT_ERROR:
                std::cerr << "MAC data has invalid format" << std::endl;
                break;
            case 0:
                std::cout << "MAC validation suceeded for all files!"
                          << std::endl;
                break;
        }
        input.close();
        return result;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        input.close();
        return HARD_FAIL;
    }
}
