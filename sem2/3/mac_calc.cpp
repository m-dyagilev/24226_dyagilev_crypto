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

int validateMACs(std::ostream& oOutput, const std::filesystem::path& iDirPath,
                 const std::array<uint8_t, ConstParams::KEY_LEN_BYTES>& iKey) {
    json jsonOutput;
    for (const std::filesystem::directory_entry& dirent :
         std::filesystem::directory_iterator(iDirPath)) {
        if (!dirent.is_regular_file()) {
            continue;
        }

        std::array<uint8_t, ConstParams::NONCE_LEN_BYTES> nonce;
        CryptoPP::AutoSeededRandomPool rng;
        rng.GenerateBlock(nonce.data(), nonce.size());

        std::ifstream curFile(dirent.path(), std::ios::binary | std::ios::in);
        if (!curFile.is_open()) {
            return FILE_ACCESS_ERROR;
        }
        std::vector<uint8_t> rawData;
        rawData.reserve(nonce.size() +
                        std::filesystem::file_size(dirent.path()));
        for (uint8_t byte : nonce) {
            rawData.push_back(byte);
        }
        while (curFile.good()) {
            uint8_t byte;
            curFile >> byte;
            rawData.push_back(byte);
        }
        curFile.close();

        std::vector<uint8_t> mac;

        CryptoPP::HMAC<CryptoPP::SHA256> hmac(iKey.data(), iKey.size());
        CryptoPP::VectorSource vsForMAC(
            rawData, true,
            new CryptoPP::HashFilter(hmac, new CryptoPP::VectorSink(mac)));

        std::string nonceHexView;
        CryptoPP::ArraySource asForNonceHexView(
            nonce.data(), nonce.size(), true,
            new CryptoPP::HexEncoder(new CryptoPP::StringSink(nonceHexView)));

        std::string macHexView;
        CryptoPP::VectorSource vsForMACHexView(
            mac, true,
            new CryptoPP::HexEncoder(new CryptoPP::StringSink(macHexView)));

        json jsonDataForCurrentFile;
        jsonDataForCurrentFile["path"] = dirent.path().c_str();
        jsonDataForCurrentFile["nonce"] = nonceHexView;
        jsonDataForCurrentFile["mac"] = macHexView;
        jsonOutput.push_back(jsonDataForCurrentFile);
    }
    oOutput << jsonOutput.dump(2);
    return 0;
}

int main(int argc, char** argv) {
    if (4 > argc) {
        std::cerr << "Need 3 arguments: "
                  << "path to directory that it is needed to process, "
                  << "key and output filenames" << std::endl;
        return NOT_ENOUGH_ARGS;
    }
    std::filesystem::path iDirPath(argv[1]);
    std::filesystem::path keyPath(argv[2]);
    std::ifstream keyfile(keyPath, std::ios::binary | std::ios::in);
    if (!keyfile.is_open()) {
        std::cerr << "Cannot open key file " << argv[2] << std::endl;
        return FILE_NOT_OPENED;
    }
    std::ofstream output(argv[3]);
    if (!output.is_open()) {
        std::cerr << "Cannot open output file " << argv[3] << std::endl;
        keyfile.close();
        return FILE_NOT_OPENED;
    }
    size_t keyLen = std::filesystem::file_size(keyPath);
    if (keyLen != ConstParams::KEY_LEN_BYTES) {
        std::cerr << "Key must have length " << ConstParams::KEY_LEN_BYTES
                  << ", but has " << keyLen << std::endl;
        keyfile.close();
        output.close();
        return INCOMPATIBLE_KEY;
    }

    std::array<uint8_t, ConstParams::KEY_LEN_BYTES> key;
    if (readKeyFromFile(key, keyfile)) {
        std::cerr << "Failed reading key" << std::endl;
        keyfile.close();
        output.close();
        return FILE_ACCESS_ERROR;
    }
    keyfile.close();

    try {
        int result = validateMACs(output, iDirPath, key);
        switch (result) {
            case FILE_ACCESS_ERROR:
                std::cerr << "File access error" << std::endl;
                break;
        }
        output.close();
        return result;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        output.close();
        return HARD_FAIL;
    }
}
