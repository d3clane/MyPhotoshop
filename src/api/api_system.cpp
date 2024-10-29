#include "api/api_system.hpp"

#include <algorithm>
#include <math.h>


namespace psapi
{


namespace sfm
{


Color::Color(uint8_t init_r, uint8_t init_g, uint8_t init_b, uint8_t init_a /*= 255u*/)
:   r(init_r), g(init_g), b(init_b), a(init_a) {}


Color &Color::operator+=(const Color &color)
{
    r = uint8_t(std::min(uint16_t(r + color.r), uint16_t(255)));
    g = uint8_t(std::min(uint16_t(g + color.g), uint16_t(255)));
    b = uint8_t(std::min(uint16_t(b + color.b), uint16_t(255)));
    a = std::max(a, color.a);

    return *this;
}


Color &Color::operator*=(const Color &color)
{
    r = uint8_t(std::min(uint16_t(r * color.r), uint16_t(255)));
    g = uint8_t(std::min(uint16_t(g * color.g), uint16_t(255)));
    b = uint8_t(std::min(uint16_t(b * color.b), uint16_t(255)));
    a = std::max(a, color.a);

    return *this;
}


Color &Color::operator*=(const double cf)
{
    r = uint8_t(std::min(uint16_t(r * cf), uint16_t(255)));
    g = uint8_t(std::min(uint16_t(g * cf), uint16_t(255)));
    b = uint8_t(std::min(uint16_t(b * cf), uint16_t(255)));

    return *this;
}


Color &Color::operator*=(const float cf)
{
    r = uint8_t(std::min(uint16_t(r * cf), uint16_t(255)));
    g = uint8_t(std::min(uint16_t(g * cf), uint16_t(255)));
    b = uint8_t(std::min(uint16_t(b * cf), uint16_t(255)));

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

} // sfm


} // psapi