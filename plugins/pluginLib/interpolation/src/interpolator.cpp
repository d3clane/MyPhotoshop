#include "interpolation/include/interpolator.hpp"

#include <iostream>

namespace ps
{

void Interpolator::push(const vec2d& point)
{
    points_.push_back(point);
    if (points_.size() > kCatmullRomPoints)
    {
        points_.pop_front();
    }

    if (isPossibleToDraw())
    {
        interpolation_ = CatmullRomInterpolation(
            points_[0], points_[1], points_[2], points_[3]
        );
    }
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
    return points_.size() == kCatmullRomPoints;
}


} // namespace ps