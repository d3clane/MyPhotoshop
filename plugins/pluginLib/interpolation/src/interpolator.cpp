#include "interpolation/include/interpolator.hpp"

#include <iostream>

namespace ps
{

void Interpolator::updateInterpolation()
{
    if (isPossibleToDraw())
    {
        interpolation_ = CatmullRomInterpolation(
            points_[0], points_[1], points_[2], points_[3]
        );
    }
}

void Interpolator::pushBack(const vec2d& point)
{
    points_.push_back(point);
    updateInterpolation();
}

void Interpolator::popFront()
{
    points_.pop_front();
    updateInterpolation();
}

void Interpolator::clear()
{
    points_.clear();
    interpolation_ = CatmullRomInterpolation();
}

vec2d Interpolator::operator[](double pos) const
{
    return interpolation_[pos];
}

bool Interpolator::isPossibleToDraw() const
{
    return points_.size() >= kCatmullRomPoints;
}


} // namespace ps