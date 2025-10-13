#include <fstream>
#include <iostream>
#include <map>

#include "certificate.hpp"

enum ERRORS {
    NOT_ENOUGH_ARGS = 1,
    FILE_NOT_OPENED,
    INVALID_FILE,
};

static const char* ROOT_CENTER_NAME = "root";

bool fillCentersHierarchy(
    std::istream& iInputStream,
    std::map<std::string, std::string>& oOwnerToSignedByMap) {
    if (!iInputStream.good()) {
        return false;
    }
    while (iInputStream.good()) {
        std::string owner;
        iInputStream >> owner;
        if (!iInputStream.good()) {
            return false;
        }
        std::string signedBy;
        iInputStream >> signedBy;
        oOwnerToSignedByMap[owner] = signedBy;
    }
    return true;
}

bool validateCertificate(
    const std::map<std::string, std::string>& iOwnerToSignedByMap,
    const Certificate& iCertificate) {
    std::string curCenter = iCertificate.signedBy;
    while (ROOT_CENTER_NAME != curCenter) {
        const auto it = iOwnerToSignedByMap.find(curCenter);
        if (iOwnerToSignedByMap.end() == it) {
            return false;
        }
        std::cout << curCenter << " --> " << it->second << std::endl;
        curCenter = it->second;
    }
    return true;
}

int main(int argc, char** argv) {
    if (4 > argc) {
        std::cerr
            << "Need 3 arguments: filename for list of known centers "
            << "and data of a certificate to check: owner name and center name "
            << std::endl;
        return NOT_ENOUGH_ARGS;
    }
    std::ifstream input(argv[1]);
    if (!input.is_open()) {
        std::cerr << "Cannot open input file " << argv[1] << std::endl;
        return FILE_NOT_OPENED;
    }
    Certificate certToCheck{argv[2], argv[3]};
    std::map<std::string, std::string> ownerToSignedByMap;
    if (!fillCentersHierarchy(input, ownerToSignedByMap)) {
        std::cerr << "Error while reading file: seems it is invalid"
                  << std::endl;
        return INVALID_FILE;
    }
    input.close();
    if (validateCertificate(ownerToSignedByMap, certToCheck)) {
        std::cout << "Path to " << ROOT_CENTER_NAME
                  << " found, certificate is valid" << std::endl;
    } else {
        std::cout << "Path to " << ROOT_CENTER_NAME
                  << " not found, certificate is invalid" << std::endl;
    }
    return 0;
}
