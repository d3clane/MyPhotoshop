#ifndef SYSTEM_ELEMENTS_API
#define SYSTEM_ELEMENTS_API


#include <cstdint>
#include <cmath>


namespace psapi
{

namespace sfm
{


template<typename T>
struct Vec2D
{
    T x;
    T y;
    Vec2D<T>(T init_x, T init_y)
        :   x(init_x), y(init_y) {}
    Vec2D<T>() = default;
};

using vec2i = Vec2D<int>;
using vec2u = Vec2D<unsigned int>;
using vec2f = Vec2D<float>;
using vec2d = Vec2D<double>;


template<typename T>
Vec2D<T> &operator+=(Vec2D<T> &lhs, const Vec2D<T> &rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;

    return lhs;
}


template<typename T>
Vec2D<T> &operator-=(Vec2D<T> &lhs, const Vec2D<T> &rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;

    return lhs;
}


template<typename T>
Vec2D<T> &operator*=(Vec2D<T> &lhs, const Vec2D<T> &rhs)
{
    lhs.x *= rhs.x;
    lhs.y *= rhs.y;

    return lhs;
}


template<typename T>
Vec2D<T> &operator*=(Vec2D<T> &lhs, double cf)
{
    lhs.x += static_cast<T>(cf);
    lhs.y += static_cast<T>(cf);

    return lhs;
}


template<typename T>
double len(const Vec2D<T> &x, const Vec2D<T> &y)
{
    return std::sqrt(len2(x, y));
}


template<typename T>
double len2(const Vec2D<T> &x, const Vec2D<T> &y)
{
    return (x.x - y.x) * (x.x - y.x) + (x.y - y.y) * (x.y - y.y);
}


template<typename T>
double len(const Vec2D<T> &x)
{
    return len(x, Vec2D<T>(T(), T()));
}


template<typename T>
double len2(const Vec2D<T> &x)
{
    return len2(x, Vec2D<T>(T(), T()));
}


template<typename T>
Vec2D<T> operator+(const Vec2D<T> &x, const Vec2D<T> &y)
{
    return Vec2D<T>(x.x + y.x, x.y + y.y);
}


template<typename T>
Vec2D<T> operator-(const Vec2D<T> &x, const Vec2D<T> &y)
{
    return Vec2D<T>(x.x - y.x, x.y - y.y);
}


template<typename T>
Vec2D<T> operator*(const Vec2D<T> &x, const Vec2D<T> &y)
{
    return Vec2D<T>(x.x * y.x, x.y * y.y);
}


template<typename T>
Vec2D<T> operator*(const Vec2D<T> &x, double cf)
{
    return Vec2D<T>(x.x * static_cast<T>(cf), x.y * static_cast<T>(cf));
}


template<typename T>
Vec2D<T> operator*(double cf, const Vec2D<T> &x)
{
    return x * cf;
}


struct Color
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    Color() = default;
    Color(uint8_t init_r, uint8_t init_g, uint8_t init_b, uint8_t init_a = 255u);

    Color &operator+=(const Color &color);
    Color &operator*=(const Color &color);
    Color &operator*=(const double cf);
    Color &operator*=(const float cf);
};

Color operator+(const Color &x, const Color &y);
Color operator*(const Color &x, const Color &y);
Color operator*(const Color &x, const float cf);
Color operator*(const Color &x, const double cf);
Color operator*(const float cf, const Color &x);
Color operator*(const double cf, const Color &x);


} // sfm

} // psapi


#endif // SYSTEM_ELEMENTS_API