#include <iomanip>
#include <iostream>
#include <set>

#include "EllipCurveUtils.hpp"

enum ERRORS {
    NOT_ENOUGH_ARGS = 1,
    INVALID_PARAMETER,
};

void demonstrateLaw(size_t a, size_t b, size_t p) {
    CurveParams curveParams{a, b};
    std::vector<Point> curvePoints = getAllPoints(curveParams, p);
    const size_t WIDTH = 6;
    std::cout << std::setw(WIDTH) << "+";
    std::cout << "|";
    for (size_t i = 0; i < curvePoints.size(); ++i) {
        std::cout << curvePoints[i] << " ";
    }
    std::cout << std::endl;
    for (size_t i = 0; i < (curvePoints.size() + 1) * WIDTH + 1; ++i) {
        std::cout << "-";
    }
    std::cout << std::endl;
    for (size_t i = 0; i < curvePoints.size(); ++i) {
        std::cout << curvePoints[i] << " ";
        std::cout << "|";
        for (size_t j = 0; j < curvePoints.size(); ++j) {
            std::cout << addPoints(curveParams, 
                curvePoints[i], curvePoints[j], p) << " ";
        }
        std::cout << std::endl;
    }
}

void demonstrateOrders(size_t a, size_t b, size_t p) {
    CurveParams curveParams{a, b};
    std::vector<Point> curvePoints = getAllPoints(curveParams, p);
    const size_t WIDTH = 6;
    std::cout << std::left;
    std::cout << std::setw(WIDTH) << "P" << "|";
    for (size_t i = 1; i < p; ++i) {
        std::cout << "[" << i << std::setw(WIDTH - 2) << "]P ";
    }
    std::cout << "|" << std::endl;
    for (size_t i = 1; i < p * WIDTH + 2; ++i) {
        std::cout << "-";
    }
    std::cout << "|" << std::endl;
    for (size_t i = 0; i < curvePoints.size(); ++i) {
        std::set<Point> valuesMet;
        std::cout << curvePoints[i] << " |";
        bool calc = true;
        for (size_t m = 1; m < p; ++m) {
            if (calc) {
                Point curValue = mComposePoint(curveParams, curvePoints[i], p, m);
                if (valuesMet.count(curValue)) {
                    calc = false;
                    for (size_t k = 0; k < WIDTH; ++k) {
                        std::cout << " ";
                    }
                } else {
                    valuesMet.insert(curValue);
                    std::cout << curValue << " ";
                }
            } else {
                for (size_t k = 0; k < WIDTH; ++k) {
                    std::cout << " ";
                }
            }
        }
        size_t count = valuesMet.size();
        std::cout << "|" << count;
        if (p - 1 == count) {
            std::cout << " - generator!";
        }
        std::cout << std::endl;
    }
}

int main(int argc, char** argv) {
    if (4 > argc) {
        std::cerr << "Need 3 arguments: P, a, b" << std::endl;
        return NOT_ENOUGH_ARGS;
    }
    long long p = std::atoll(argv[1]);
    if ((2 >= p)) {
        std::cerr << "Invalid P, must be a prime number >2" << std::endl;
        return INVALID_PARAMETER;
    }
    long long a = std::atoll(argv[2]);
    if ((1 > a)) {
        std::cerr << "Invalid a, must be a number >=1" << std::endl;
        return INVALID_PARAMETER;
    }
    long long b = std::atoll(argv[3]);
    if ((1 > b)) {
        std::cerr << "Invalid b, must be a number >=1" << std::endl;
        return INVALID_PARAMETER;
    }
    demonstrateOrders(a, b, p);
}
