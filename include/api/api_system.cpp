#include "api_system.hpp"

#include <algorithm>
#include <math.h>


namespace psapi
{


namespace sfm
{

vec2u vec2iToVec2u(vec2i v)
{
    return vec2u(static_cast<unsigned int>(v.x), static_cast<unsigned int>(v.y));
}


vec2i vec2dToVec2i(vec2d v)
{
    return vec2i(static_cast<int>(v.x), static_cast<int>(v.y));
}

vec2u vec2dToVec2u(vec2d v)
{
    return vec2u(static_cast<unsigned>(v.x), static_cast<unsigned>(v.y));
}


static uint8_t findAverage(uint8_t a, uint8_t b)
{
    return static_cast<uint8_t>((static_cast<int>(a) + static_cast<int>(b)) / 2);
}

int clampColors(int val)
{
    return std::min(std::max(0, val), 255);
}

static uint8_t sumColors(uint8_t a, uint8_t b)
{
    return static_cast<uint8_t>(clampColors(static_cast<int>(a) + static_cast<int>(b)));
}

static uint8_t subColors(uint8_t a, uint8_t b)
{
    return static_cast<uint8_t>(clampColors(static_cast<int>(a) - static_cast<int>(b)));
}


Color::Color(uint8_t init_r, uint8_t init_g, uint8_t init_b, uint8_t init_a /*= 255u*/)
:   r(init_r), g(init_g), b(init_b), a(init_a) {}


Color &Color::operator+=(const Color &color)
{
    r = sumColors(r, color.r);
    g = sumColors(g, color.g);
    b = sumColors(b, color.b);
    a = std::max(a, color.a);

    return *this;
}

Color &Color::operator*=(const Color &color)
{
    double rf = r * color.r;
    double gf = g * color.g;
    double bf = b * color.b;

    double max_c = std::max(std::max(1.0, rf), std::max(gf, bf));

    r = static_cast<uint8_t>(rf / max_c * 255.0);
    g = static_cast<uint8_t>(gf / max_c * 255.0);
    b = static_cast<uint8_t>(bf / max_c * 255.0);
    a = std::max(a, color.a);

    return *this;
}


Color &Color::operator*=(const double cf)
{
    double rf = r * cf;
    double gf = g * cf;
    double bf = b * cf;

    double max_c = std::max(std::max(1.0, rf), std::max(gf, bf));

    r = static_cast<uint8_t>(rf / max_c * 255.0);
    g = static_cast<uint8_t>(gf / max_c * 255.0);
    b = static_cast<uint8_t>(bf / max_c * 255.0);

    return *this;
}


Color &Color::operator-=(const Color &color)
{
    r = subColors(r, color.r);
    g = subColors(g, color.g);
    b = subColors(b, color.b);
    a = std::max(a, color.a);

    return *this;
}


Color &Color::operator*=(const float cf)
{
    double rf = r * cf;
    double gf = g * cf;
    double bf = b * cf;

    double max_c = std::max(rf, std::max(gf, bf));

    r = static_cast<uint8_t>(rf / max_c * 255.0);
    g = static_cast<uint8_t>(gf / max_c * 255.0);
    b = static_cast<uint8_t>(bf / max_c * 255.0);

    return *this;
}


Color operator+(const Color &x, const Color &y)
{
    Color tmp = x;
    tmp += y;
    return tmp;
}


Color operator*(const Color &x, const Color &y)
{
    Color tmp = x;
    tmp *= y;
    return tmp;
}


Color operator*(const Color &x, const float cf)
{
    Color tmp = x;
    tmp *= cf;
    return tmp;
}


Color operator*(const Color &x, const double cf)
{
    Color tmp = x;
    tmp *= cf;
    return tmp;
}


Color operator*(const float cf, const Color &x)
{
    return x * cf;
}


Color operator*(const double cf, const Color &x)
{
    return x * cf;
}

Color mix(const Color &x, const Color &y)
{
    Color tmp = x;

    tmp.r = findAverage(x.r, y.r);
    tmp.g = findAverage(x.g, y.g);
    tmp.b = findAverage(x.b, y.b);
    tmp.a = std::max(x.a, y.a);

    return tmp;
}

} // sfm


} // psapi
