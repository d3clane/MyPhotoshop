#include "interpolation/include/interpolator.hpp"

#include <iostream>

namespace ps
{

namespace 
{

bool isEqual(const vec2d& point1, const vec2d& point2)
{
    return point1.x == point2.x && point1.y == point2.y;
}

}
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
    bool ignoring = 
        (points_.size() >= 1 && isEqual(points_[points_.size() - 1], point)) || 
        (points_.size() >= 2 && isEqual(points_[points_.size() - 2], point)) || 
        (points_.size() >= 3 && isEqual(points_[points_.size() - 3], point));

    if (!ignoring)
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