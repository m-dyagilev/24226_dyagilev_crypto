#include <algorithm>
#include <iomanip>
#include <iostream>
#include <vector>

#include "CorrectModulus.hpp"
#include "EllipCurveUtils.hpp"
#include "ExtEuc.hpp"

int main(int argc, char** argv) {
    {
        CurveParams curveParams{45, 27};
        size_t p = 53;
        Point pnt{6, 47};
        size_t m = 120;
        Point composition = mComposePoint(curveParams, pnt, p, m);
        std::cout << "[" << m << "]" << pnt << " == " << composition
                  << std::endl;
    }
    {
        CurveParams curveParams{1, 3};
        size_t p = 7;
        Point pnt{4, 1};
        size_t m = 1;
        Point composition = mComposePoint(curveParams, pnt, p, m);
        std::cout << "[" << m << "]" << pnt << " == " << composition
                  << std::endl;
        m = 2;
        composition = mComposePoint(curveParams, pnt, p, m);
        std::cout << "[" << m << "]" << pnt << " == " << composition
                  << std::endl;
        m = 3;
        composition = mComposePoint(curveParams, pnt, p, m);
        std::cout << "[" << m << "]" << pnt << " == " << composition
                  << std::endl;
        m = 4;
        composition = mComposePoint(curveParams, pnt, p, m);
        std::cout << "[" << m << "]" << pnt << " == " << composition
                  << std::endl;
        m = 5;
        composition = mComposePoint(curveParams, pnt, p, m);
        std::cout << "[" << m << "]" << pnt << " == " << composition
                  << std::endl;
        m = 6;
        composition = mComposePoint(curveParams, pnt, p, m);
        std::cout << "[" << m << "]" << pnt << " == " << composition
                  << std::endl;
    }
}
