#include "EllipCurveUtils.hpp"

#include <stdexcept>
#include <cassert>

#include "CorrectModulus.hpp"
#include "ExtEuc.hpp"

std::ostream& operator<<(std::ostream& os, Point pnt) {
    os << "(" << pnt.x << "," << pnt.y << ")";
    return os;
}

bool operator<(const Point& lhs, const Point& rhs) noexcept {
    return std::tie(lhs.x, lhs.y) < std::tie(rhs.x, rhs.y);
}

Point addPoints(const CurveParams& iCurveParams, const Point& iPoint1,
                const Point& iPoint2, size_t p) {
    if (0 == iPoint1.x && 0 == iPoint1.y) {
        return iPoint2;
    }
    if (0 == iPoint2.x && 0 == iPoint2.y) {
        return iPoint1;
    }
    size_t denom = modulus(iPoint2.x - iPoint1.x, p);
    size_t num = modulus(iPoint2.y - iPoint1.y, p);
    if (0 == denom) {
        if (0 == num) {
            return doublePoint(iCurveParams, iPoint1, p);
        }
        if (0 == modulus(iPoint1.y + iPoint2.y, p)) {
            return Point{0, 0};
        }
    }
    size_t invDenom;
    if (getInverse(denom, p, invDenom)) {
        throw std::logic_error("Inversion failed");
    }
    size_t k = modulus(num * invDenom, p);
    Point result;
    result.x =
        modulus(modulus(modulus(k * k, p) - iPoint1.x, p) - iPoint2.x, p);
    result.y = modulus(
        modulus(modulus(iPoint1.x - result.x, p) * k, p) - iPoint1.y, p);
    return result;
}

#include <iostream>

Point doublePoint(const CurveParams& iCurveParams, const Point& iPoint,
                  size_t p) {
    if (0 == iPoint.x && 0 == iPoint.y) {
        return iPoint;
    }
    if (0 == iPoint.y) {
        return Point{0, 0};
    }
    size_t num =
        modulus(modulus(3 * iPoint.x * iPoint.x, p) + iCurveParams.a, p);
    size_t denom = modulus(2 * iPoint.y, p);
    size_t invDenom;
    if (getInverse(denom, p, invDenom)) {
        throw std::logic_error("Inversion failed");
    }
    size_t k = modulus(num * invDenom, p);
    // std::cout << "<num, denom, invDenom, k>: <" << num << ", " << denom 
    // << ", " << invDenom << ", " << k << ">" << std::endl;
    Point result;
    result.x = modulus(modulus(k * k, p) - modulus(2 * iPoint.x, p), p);
    result.y =
        modulus(modulus(modulus(iPoint.x - result.x, p) * k, p) - iPoint.y, p);
    return result;
}

#define BIT(x) (1 << x)

#include <iostream>

Point mComposePoint(const CurveParams& iCurveParams, const Point& iPoint,
                    size_t p, size_t m) {
    // Point result{0, 0};
    // while (0 < m) {
    //     std::cout << m << std::endl;
    //     std::cout << "result pre: " << result << std::endl;
    //     result = doublePoint(iCurveParams, result, p);
    //     std::cout << "result 1: " << result << std::endl;
    //     if (m & BIT(i)) {
    //         result = addPoints(iCurveParams, result, iPoint, p);
    //         std::cout << "result 2: " << result << std::endl;
    //     }
    // }
    // return result;
    Point result{0, 0};
    if (0 == iPoint.x && 0 == iPoint.y) {
        return result;
    }
    size_t mCopy = m;
    size_t numBits = 0;
    while (0 < mCopy) {
        mCopy >>= 1;
        numBits++;
    }
    // std::cout << m << " is " << numBits << " long" << std::endl;
    for (size_t i = numBits; i > 0; --i) {
        // std::cout << i << std::endl;
        // std::cout << "result pre: " << result << std::endl;
        result = doublePoint(iCurveParams, result, p);
        // std::cout << "result 1: " << result << std::endl;
        if (m & BIT((i - 1))) {
            result = addPoints(iCurveParams, result, iPoint, p);
            // std::cout << "result 2: " << result << std::endl;
        }
    }
    return result;
}

std::vector<Point> getAllPoints(const CurveParams& iCurveParams, size_t p) {
    assert(0 != 4 * iCurveParams.a * iCurveParams.a * iCurveParams.a
            + 27 * iCurveParams.b * iCurveParams.b);
    std::vector<Point> result;
    for (size_t i = 0; i < p; ++i) {
        for (size_t j = 0; j < p; ++j) {
            Point pnt{i, j};
            if (pointBelongsToCurve(iCurveParams, pnt, p)) {
                result.push_back(pnt);
            }
        }
    }
    return result;
}

bool pointBelongsToCurve(const CurveParams& iCurveParams, const Point& iPoint,
                         size_t p) {
    if (0 == iPoint.x && 0 == iPoint.y) {
        return true;
    }
    return modulus(iPoint.y * iPoint.y, p) ==
           modulus(modulus(modulus(iPoint.x * iPoint.x * iPoint.x, p) +
                               modulus(iCurveParams.a * iPoint.x, p),
                           p) +
                       iCurveParams.b,
                   p);
}
