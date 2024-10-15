#ifndef ENGINE_VECTOR_HPP
#define ENGINE_VECTOR_HPP

#include <stddef.h>

namespace Engine
{

struct Point;

struct Vector
{
    double dx, dy, dz;

    Vector(double dx, double dy, double dz);
    Vector(const Point& begin, const Point& end);    
    
    explicit Vector(const Point& point);

    double lengthSquare() const;

    double length() const;
    void   length(const double newLength);

    bool isZero() const;
    
    Vector reflectRelatively(const Vector& pivot) const;
    Vector getNormalizedVector() const;

    Vector getPerpendicular() const;

    Vector projectOnto(const Vector& other) const;

    Vector& operator+=(const Vector& other);
    Vector& operator-=(const Vector& other);
    Vector& operator*=(const double coeff);
    Vector& operator/=(const double coeff);

    bool operator==(const Vector& other) const;
    
    operator Point() const;

    virtual ~Vector() = default;
};

Vector operator -(const Vector& self);
Vector operator +(const Vector& self, const Vector& other);
Vector operator -(const Vector& self, const Vector& other);
Vector operator *(const Vector& self, const double coeff);
Vector operator *(const double coeff, const Vector& self);
Vector operator /(const Vector& self, const double coeff);
double operator ^(const Vector& self, const Vector& other);

double cos(const Vector& v1, const Vector& v2);

} // namespace Engine

#endif // ENGINE_VECTOR_HPP