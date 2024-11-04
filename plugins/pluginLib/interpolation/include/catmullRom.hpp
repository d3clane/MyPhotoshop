#ifndef CATMULL_ROM_HPP
#define CATMULL_ROM_HPP

#include "api/api_system.hpp"

namespace ps
{

using psapi::sfm::vec2d;

static const size_t kCatmullRomPoints = 4;

class CatmullRomInterpolation
{
public:
    struct CatmullRomPoint
    {
        vec2d point;
        double t;
    };

    CatmullRomInterpolation() : alpha_(0.5) {}
    CatmullRomInterpolation(
        const vec2d& p1, const vec2d& p2, 
        const vec2d& p3, const vec2d& p4, double alpha = 0.5
    );
    
    void setPoint(const vec2d& newPoint, int pos);

    vec2d operator[](double pos) const;
    vec2d& operator[](int pos);
private:
    CatmullRomPoint points_[kCatmullRomPoints];
    double alpha_;
};

} // namespace ps


#endif // CATMULL_ROM_HPP