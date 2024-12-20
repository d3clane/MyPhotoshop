#ifndef CATMULL_ROM_H_
#define CATMULL_ROM_H_

#include "api/api_sfm.hpp"

#include <vector>

using namespace psapi::sfm;

namespace cr
{

bool operator==(const vec2f& a, const vec2f& b);

class CatmullRom {
private:
    constexpr float Eval_h00(float t);
    constexpr float Eval_h01(float t);
    constexpr float Eval_h10(float t);
    constexpr float Eval_h11(float t);

    std::vector<vec2f> points_;

    vec2f Eval_m(const vec2f p0,
                       const vec2f p1);

    vec2f InterpolatePoint(float t, const vec2f& p0, const vec2f& p1,
                            const vec2f& p2, const vec2f& p3) const;

public:
    CatmullRom(const std::vector<vec2f>& points)
        : points_(points) {}

    void addPoint(const vec2f& point);
    void removePoint(const vec2f& point);
    void setPoint(const vec2f& prevPoint, const vec2f& newPoint);

    size_t getSize() const;

    vec2f operator[](float t) const;
};

} // namespace cr

#endif // CATMULL_ROM_H_