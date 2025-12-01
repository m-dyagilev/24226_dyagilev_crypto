#include <algorithm>
#include <iomanip>
#include <iostream>
#include <vector>

#include "CorrectModulus.hpp"
#include "ExtEuc.hpp"
#include "EllipCurveUtils.hpp"

int main(int argc, char** argv) {
    // ExtEucResult result;
    // int status = getBareExtEucResult(53, 41, result);
    // std::cout << (status ? "Not ok" : "ok") << std::endl;
    // std::cout << "gcd: " << result.gcd << std::endl;
    // size_t inv = 0;
    // status = getInverseFromEucResult(result, 53, inv);
    // std::cout << (status ? "Not ok" : "ok") << std::endl;
    // std::cout << "inv: " << inv << std::endl;
    {
        CurveParams curveParams{45, 27};
        size_t p = 53;
        Point pnt{6, 47};
        size_t m = 120;
        Point composition = mComposePoint(curveParams, pnt, p, m);
        std::cout << "[" << m << "]" << pnt << " == " << composition << std::endl;
    }
    {
        CurveParams curveParams{1, 3};
        size_t p = 7;
        Point pnt{4, 1};
        size_t m = 1;
        Point composition = mComposePoint(curveParams, pnt, p, m);
        std::cout << "[" << m << "]" << pnt << " == " << composition << std::endl;
        m = 2;
        composition = mComposePoint(curveParams, pnt, p, m);
        std::cout << "[" << m << "]" << pnt << " == " << composition << std::endl;
        m = 3;
        composition = mComposePoint(curveParams, pnt, p, m);
        std::cout << "[" << m << "]" << pnt << " == " << composition << std::endl;
        m = 4;
        composition = mComposePoint(curveParams, pnt, p, m);
        std::cout << "[" << m << "]" << pnt << " == " << composition << std::endl;
        m = 5;
        composition = mComposePoint(curveParams, pnt, p, m);
        std::cout << "[" << m << "]" << pnt << " == " << composition << std::endl;
        m = 6;
        composition = mComposePoint(curveParams, pnt, p, m);
        std::cout << "[" << m << "]" << pnt << " == " << composition << std::endl;
    }
    // std::cout << modulus(-22, 53) << std::endl;
}
