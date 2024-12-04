#include <cassert>
#include <cmath>

#include <iostream>

#include "pluginLib/interpolation/include/catmullRom.hpp"

namespace ps
{

namespace 
{

vec2d calcIntermidiateVal(
    const double tLeftNumerator,  const double tLeftDenominator1,  const double tLeftDenominator2,  const vec2d& leftPoint,
    const double tRightNumerator, const double tRightDenominator1, const double tRightDenominator2, const vec2d& rightPoint,
    const double posT
)
{
    return (tLeftNumerator - posT)  / (tLeftDenominator1  - tLeftDenominator2 ) * leftPoint + 
           (posT - tRightNumerator) / (tRightDenominator1 - tRightDenominator2) * rightPoint;
}

vec2d calcCatmullRomPoint(
    const CatmullRomInterpolation::CatmullRomPoint points[kCatmullRomPoints], const double posT
)
{
    vec2d a3 = calcIntermidiateVal(
        points[3].t, points[3].t, points[2].t, points[2].point,
        points[2].t, points[3].t, points[2].t, points[3].point, posT
    );
    vec2d a2 = calcIntermidiateVal(
        points[2].t, points[2].t, points[1].t, points[1].point,
        points[1].t, points[2].t, points[1].t, points[2].point, posT
    );
    vec2d a1 = calcIntermidiateVal(
        points[1].t, points[1].t, points[0].t, points[0].point,
        points[0].t, points[1].t, points[0].t, points[1].point, posT
    );
    vec2d b2 = calcIntermidiateVal(
        points[3].t, points[3].t, points[1].t, a2,
        points[1].t, points[3].t, points[1].t, a3, posT
    );
    vec2d b1 = calcIntermidiateVal(
        points[2].t, points[2].t, points[0].t, a1,
        points[0].t, points[2].t, points[0].t, a2, posT
    );

    return calcIntermidiateVal(
        points[2].t, points[2].t, points[1].t, b1, 
        points[1].t, points[2].t, points[1].t, b2, posT
    );
}

double calcNextT(const vec2d& prevPoint, const vec2d& nextPoint, double prevT, const double alpha)
{
    double dx = nextPoint.x - prevPoint.x;
    double dy = nextPoint.y - prevPoint.y;

    return std::pow(std::sqrt(dx * dx + dy * dy), alpha) + prevT;
}

} // namespace anonymous

CatmullRomInterpolation::CatmullRomInterpolation(
    const vec2d& p1, const vec2d& p2, 
    const vec2d& p3, const vec2d& p4, double alpha
) : alpha_(alpha) 
{
    points_[0].point = p1;
    points_[1].point = p2;
    points_[2].point = p3;
    points_[3].point = p4;

    points_[0].t = 0.0;

    for (size_t i = 1; i < kCatmullRomPoints; ++i)
        points_[i].t = calcNextT(points_[i - 1].point, points_[i].point, points_[i - 1].t, alpha_);
}

vec2d CatmullRomInterpolation::operator[](double pos) const
{
    int posInPointsArray = (int)pos;
    double deltaToPosInArray = pos - posInPointsArray;
 
    assert(posInPointsArray >= 0);
    assert(posInPointsArray < 3);

    double dt = points_[posInPointsArray + 1].t - points_[posInPointsArray].t;
    double t  = points_[posInPointsArray].t + deltaToPosInArray * dt;

    return calcCatmullRomPoint(points_, t);
}

vec2d& CatmullRomInterpolation::operator[](int pos)
{
    assert(pos < 3);
    assert(pos >= 0);

    return points_[pos].point;
}

void CatmullRomInterpolation::setPoint(const vec2d& newPoint, int pos)
{
    assert(pos <= 3);
    assert(pos >= 0);

    points_[pos].point = newPoint;
}

} // namespace ps
