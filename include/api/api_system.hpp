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

    Vec2D() = default;

    Vec2D(T init_x, T init_y)
        :   x(init_x), y(init_y) {}

    template<typename U>
    Vec2D(const Vec2D<U>& vec,
          typename std::enable_if<std::is_same<T, int>::value && std::is_same<U, unsigned int>::value>::type* = nullptr)
        : x(static_cast<T>(vec.x)), y(static_cast<T>(vec.y)) {}

    template<typename U>
    Vec2D(const Vec2D<U>& vec,
          typename std::enable_if<std::is_same<T, float>::value && std::is_same<U, double>::value>::type* = nullptr)
        : x(static_cast<T>(vec.x)), y(static_cast<T>(vec.y)) {}

    template<typename U>
    Vec2D(const Vec2D<U>& vec,
          typename std::enable_if<std::is_same<T, double>::value && std::is_same<U, float>::value>::type* = nullptr)
        : x(static_cast<T>(vec.x)), y(static_cast<T>(vec.y)) {}
};

using vec2i = Vec2D<int>;
using vec2u = Vec2D<unsigned int>;
using vec2f = Vec2D<float>;
using vec2d = Vec2D<double>;

vec2u vec2iToVec2u(vec2i v);
vec2i vec2dToVec2i(vec2d v);
vec2u vec2dToVec2u(vec2d v);

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
    double x = static_cast<double>(lhs.x) * cf;
    double y = static_cast<double>(lhs.y) * cf;

    lhs.x = static_cast<T>(x);
    lhs.y = static_cast<T>(y);

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
    return Vec2D<T>(static_cast<T>(static_cast<double>(x.x) * cf),
                    static_cast<T>(static_cast<double>(x.y) * cf));
}


template<typename T>
Vec2D<T> operator*(double cf, const Vec2D<T> &x)
{
    return x * cf;
}


struct Color
{
    enum class Type
    {
        Black,
        White,
        Red,
        Green,
        Blue,
        Yellow,
        Magenta,
        Cyan,
        Transparent,
    };

    static Color getStandardColor(Type color);

    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 0;

    Color(uint8_t init_r, uint8_t init_g, uint8_t init_b, uint8_t init_a = 255u);
    Color() = default;

    Color &operator+=(const Color &color);
    Color &operator*=(const Color &color);
    Color &operator-=(const Color &color);
    Color &operator*=(const double cf);
    Color &operator*=(const float cf);

};


Color mix(const Color &x, const Color &y);

Color operator+(const Color &x, const Color &y);
Color operator*(const Color &x, const Color &y);
Color operator*(const Color &x, const float cf);
Color operator*(const Color &x, const double cf);
Color operator*(const float cf, const Color &x);
Color operator*(const double cf, const Color &x);


} // sfm

} // psapi


#endif // SYSTEM_ELEMENTS_API
