#ifndef ELLIPCURVEUTILS_H
#define ELLIPCURVEUTILS_H

#include <stddef.h>
#include <vector>
#include <tuple>

#include <ostream>

struct Point {
    size_t x;
    size_t y;

    friend bool operator<(const Point& lhs, const Point& rhs) noexcept;
};

std::ostream& operator<<(std::ostream& os, Point pnt);

// X^3 + aX +b
struct CurveParams {
    size_t a;
    size_t b;
};

Point addPoints(const CurveParams& iCurveParams, const Point& iPoint1,
                const Point& iPoint2, size_t p);

Point doublePoint(const CurveParams& iCurveParams, const Point& iPoint,
                  size_t p);

Point mComposePoint(const CurveParams& iCurveParams, const Point& iPoint,
                    size_t p, size_t m);

bool pointBelongsToCurve(const CurveParams& iCurveParams, const Point& iPoint,
                         size_t p);

std::vector<Point> getAllPoints(const CurveParams& iCurveParams, size_t p);

#endif
