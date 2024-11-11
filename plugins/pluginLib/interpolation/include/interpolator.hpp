#ifndef PLUGIN_LIB_INTERPOLATION_INTERPOLATOR_HPP
#define PLUGIN_LIB_INTERPOLATION_INTERPOLATOR_HPP

#include <deque>

#include "catmullRom.hpp"

namespace ps
{

class Interpolator
{
public:
    void pushBack(const vec2d& point);
    void popFront();

    void clear();

    bool isPossibleToDraw() const;

    vec2d operator[](double pos) const;

private:
    void updateInterpolation();
    
private:
    std::deque<vec2d> points_;

    CatmullRomInterpolation interpolation_;
};

} // namespace ps

#endif // PLUGIN_LIB_INTERPOLATION_INTERPOLATOR_HPP