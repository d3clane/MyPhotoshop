#include "catmullRom.hpp"

namespace cr
{

constexpr float CatmullRom::Eval_h00(float t) {
    return 2*t*t*t - 3*t*t + 1;
}

constexpr float CatmullRom::Eval_h01(float t) {
    return -2*t*t*t + 3*t*t;
}

constexpr float CatmullRom::Eval_h10(float t) {
    return t*t*t - 2*t*t + t;
}

constexpr float CatmullRom::Eval_h11(float t) {
    return t*t*t - t*t;
}

vec2f CatmullRom::Eval_m(const vec2f p0,
                               const vec2f p1) {
    return (p0 + p1) * 0.5f; 
}

vec2f CatmullRom::InterpolatePoint(float t, const vec2f& p0, const vec2f& p1,
                                                  const vec2f& p2, const vec2f& p3) {
    double t2   = t  * t,
           t3   = t2 * t,
           t3_2 = t3 * 2,
           t2_3 = t2 * 3;

    double h00 =  t3_2 - t2_3 + 1;
    double h10 =  t3   - t2 * 2 + t;
    double h01 = -t3_2 + t2_3;
    double h11 =  t3   - t2;

    vec2f p0f = vec2f( p0.x, p0.y);
    vec2f p1f = vec2f( p1.x, p1.y);
    vec2f p2f = vec2f( p2.x, p2.y);
    vec2f p3f = vec2f( p3.x, p3.y);

    vec2f m0 = (p2f - p0f) * 0.5;
    vec2f m1 = (p3f - p1f) * 0.5;

    vec2f res = h00 * p1f + h10 * m0 + h01 * p2f + h11 * m1;
    return vec2f( std::round( res.x), std::round( res.y));
}

vec2f CatmullRom::operator[](float t) {
    std::size_t point_number = static_cast<std::size_t>(t);

    return InterpolatePoint(t - static_cast<float>(point_number),
                            points_[point_number - 1],
                            points_[point_number + 0],
                            points_[point_number + 1],
                            points_[point_number + 2]);
}

bool eq(const vec2f& a, const vec2f& b) 
{
    const float eps = 0.0001f;

    return std::abs(a.x - b.x) < eps && std::abs(a.y - b.y) < eps;    
}

void CatmullRom::setPoint(const vec2f& prevPoint, const vec2f& newPoint)
{
    for (std::size_t i = 0; i < points_.size(); ++i)
    {
        if (points_[i] == prevPoint)
        {
            points_[i] = newPoint;
            break;
        }
    }
}

void CatmullRom::addPoint(const vec2f& point) { points_.push_back(point); }
void CatmullRom::removePoint(const vec2f& point) 
{
    for (std::size_t i = 0; i < points_.size(); ++i)
    {
        if (points_[i] == point)
        {
            points_.erase(points_.begin() + i);
            break;
        }
    }
}

size_t CatmullRom::getSize() const
{
    return points_.size();
}

bool operator==(const vec2f& a, const vec2f& b)
{
    return eq(a, b);
}

} // namespace cr
