#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <stack>
#include <vector>

#include "certificate.hpp"

enum ERRORS {
    NOT_ENOUGH_ARGS = 1,
    FILE_NOT_OPENED,
    INVALID_FILE,
};

struct PKINetwork {
    std::map<std::string, size_t> centerNameToIdxMap;
    std::map<size_t, std::string> centerIdxToNameMap;
    std::vector<std::vector<bool>> adjacencyMatrix;
};

static const Certificate MY_CERTIFICATE{"User3", "C3"};

bool fillPKINetwork(std::istream& iInputStream, PKINetwork& oNet) {
    if (!iInputStream.good()) {
        return false;
    }
    while (iInputStream.good()) {
        std::string center1Name;
        iInputStream >> center1Name;
        if (!iInputStream.good()) {
            return false;
        }
        std::string center2Name;
        iInputStream >> center2Name;
        size_t center1Idx;
        const auto it1 = oNet.centerNameToIdxMap.find(center1Name);
        if (oNet.centerNameToIdxMap.end() == it1) {
            center1Idx = oNet.adjacencyMatrix.size();
            for (std::vector<bool>& row : oNet.adjacencyMatrix) {
                row.push_back(false);
            }
            std::vector<bool> newRow(center1Idx + 1, false);
            oNet.adjacencyMatrix.push_back(newRow);
            oNet.centerNameToIdxMap[center1Name] = center1Idx;
            oNet.centerIdxToNameMap[center1Idx] = center1Name;
        } else {
            center1Idx = it1->second;
        }
        size_t center2Idx;
        const auto it2 = oNet.centerNameToIdxMap.find(center2Name);
        if (oNet.centerNameToIdxMap.end() == it2) {
            center2Idx = oNet.adjacencyMatrix.size();
            for (std::vector<bool>& row : oNet.adjacencyMatrix) {
                row.push_back(false);
            }
            std::vector<bool> newRow(center2Idx + 1, false);
            oNet.adjacencyMatrix.push_back(newRow);
            oNet.centerNameToIdxMap[center2Name] = center2Idx;
            oNet.centerIdxToNameMap[center2Idx] = center2Name;
        } else {
            center2Idx = it2->second;
        }
        oNet.adjacencyMatrix[center1Idx][center2Idx] = true;
        oNet.adjacencyMatrix[center2Idx][center1Idx] = true;
    }
    return true;
}

bool validateCertificate(const PKINetwork& iNet,
                         const Certificate& iCertificate,
                         std::vector<size_t>& oVisitOrder) {
    std::string curCenter = iCertificate.signedBy;
    std::stack<size_t> verticesToVisit;
    const auto it1 = iNet.centerNameToIdxMap.find(curCenter);
    if (iNet.centerNameToIdxMap.end() == it1) {
        return false;
    }
    verticesToVisit.push(it1->second);
    std::vector<bool> visited(iNet.adjacencyMatrix.size(), false);
    std::vector<int> visitFrom(iNet.adjacencyMatrix.size(), -1);
    bool found = false;
    size_t lastIdx = 0;
    while (!verticesToVisit.empty()) {
        size_t curIdx = verticesToVisit.top();
        verticesToVisit.pop();
        if (!visited[curIdx]) {
            visited[curIdx] = true;
            const auto it2 = iNet.centerIdxToNameMap.find(curIdx);
            if (iNet.centerIdxToNameMap.end() == it2) {
                return false;
            }
            if (MY_CERTIFICATE.signedBy == it2->second) {
                found = true;
                lastIdx = curIdx;
                break;
            }
            for (size_t i = 0; i < iNet.adjacencyMatrix[curIdx].size(); ++i) {
                if (iNet.adjacencyMatrix[curIdx][i]) {
                    if (!visited[i]) {
                        visitFrom[i] = curIdx;
                        verticesToVisit.push(i);
                    }
                }
            }
        }
    }
    if (found) {
        size_t curIdx = lastIdx;
        int curParent = visitFrom[lastIdx];
        while (-1 != curParent) {
            oVisitOrder.push_back(curIdx);
            curIdx = curParent;
            curParent = visitFrom[curParent];
        }
        oVisitOrder.push_back(curIdx);
        std::reverse(oVisitOrder.begin(), oVisitOrder.end());
    }
    return found;
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
    PKINetwork net;
    if (!fillPKINetwork(input, net)) {
        std::cerr << "Error while reading file: seems it is invalid"
                  << std::endl;
        return INVALID_FILE;
    }
    input.close();
    std::vector<size_t> visitOrder;
    std::cout << "My sertificate is <" << MY_CERTIFICATE.owner << ", "
              << MY_CERTIFICATE.signedBy << ">" << std::endl;
    if (validateCertificate(net, certToCheck, visitOrder)) {
        std::cout << "Path to " << MY_CERTIFICATE.signedBy
                  << " found, certificate is valid: " << std::endl;
        for (size_t i = 0; i < visitOrder.size() - 1; ++i) {
            std::cout << net.centerIdxToNameMap[visitOrder[i]] << " --> ";
        }
        std::cout << net.centerIdxToNameMap[visitOrder[visitOrder.size() - 1]]
                  << std::endl;
    } else {
        std::cout << "Path to " << MY_CERTIFICATE.signedBy
                  << " not found, certificate is invalid" << std::endl;
    }
    return 0;
}
