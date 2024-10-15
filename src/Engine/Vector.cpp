#include <cassert>
#include <cmath>

#include "Engine/Vector.hpp"
#include "Engine/CoordsSystem.hpp"

#include "Utils/Exceptions.hpp"

namespace Engine
{

double cos(const Vector& v1, const Vector& v2)
{
    if (v1.length() <= 0 || v2.length() <= 0)
    {
        throw EXCEPTION_WITH_REASON_CREATE_NEXT_EXCEPTION(
            Utils::SimulatorErrors::InvalidSize,
            "Vector length have to be more > 0 when finding cos",
            nullptr
        );
    }

    return v1 ^ v2 / v1.length() / v2.length(); 
}

Vector::Vector(double dx, double dy, double dz) : dx(dx), dy(dy), dz(dz) 
{
    if (!std::isfinite(dx) || !std::isfinite(dy) || !std::isfinite(dz))
    {
        throw EXCEPTION_WITH_REASON_CREATE_NEXT_EXCEPTION(
            Utils::SimulatorErrors::InvalidSize,
            "Vector deltas have to be finite",
            nullptr
        );
    }
}

Vector::Vector(const Point& begin, const Point& end) : 
    dx(end.x - begin.x), dy(end.y - begin.y), dz(end.z - begin.z) 
{
    if (!std::isfinite(dx) || !std::isfinite(dy) || !std::isfinite(dz))
    {
        throw EXCEPTION_WITH_REASON_CREATE_NEXT_EXCEPTION(
            Utils::SimulatorErrors::InvalidSize,
            "Vector deltas have to be finite",
            nullptr
        );
    }   
}

Vector::Vector(const Point& point) : Vector(Point{0, 0, 0}, point) 
{
    if (!std::isfinite(dx) || !std::isfinite(dy) || !std::isfinite(dz))
    {
        throw EXCEPTION_WITH_REASON_CREATE_NEXT_EXCEPTION(
            Utils::SimulatorErrors::InvalidSize,
            "Vector deltas have to be finite",
            nullptr
        );
    }
}

Vector::operator Point() const 
{ 
    return Point(dx, dy, dz); 
}

double Vector::lengthSquare() const
{
    assert(std::isfinite(dx));
    assert(std::isfinite(dy));
    assert(std::isfinite(dz));

    return dx * dx + dy * dy + dz * dz;
}

double Vector::length() const
{
    return std::sqrt(lengthSquare());    
}

void Vector::length(const double newLength)
{
    double oldLength = length();
    assert(oldLength != 0);

    double coeff = newLength / oldLength;

    dx *= coeff;
    dy *= coeff;
    dz *= coeff;
}

bool Vector::isZero() const
{
    return dx == 0 && dy == 0 && dz == 0;
}

Vector Vector::getNormalizedVector() const
{
    double len = length();

    if (len == 0)
        return Vector(0, 0, 0);

    return Vector(dx / len, dy / len, dz / len);
}

Vector Vector::reflectRelatively(const Vector& pivot) const
{
    return 2 * projectOnto(pivot) - *this;
}

Vector Vector::getPerpendicular() const
{
    return Vector(dy, -dx, 0);
}

Vector Vector::projectOnto(const Vector& other) const
{
    double myLen = length();
    if (myLen <= 0) return *this;
    
    assert(other.length() != 0);

    return other.getNormalizedVector() * cos(*this, other) * length();
}

Vector& Vector::operator+=(const Vector& other)
{
    dx += other.dx;
    dy += other.dy;
    dz += other.dz;
    return *this;
}

Vector& Vector::operator-=(const Vector& other)
{
    dx -= other.dx;
    dy -= other.dy;
    dz -= other.dz;
    return *this;
}

Vector& Vector::operator*=(const double coeff)
{
    if (!std::isfinite(coeff))
    {
        throw EXCEPTION_WITH_REASON_CREATE_NEXT_EXCEPTION(
            Utils::SimulatorErrors::InvalidSize,
            "Can't multiply vector by infinite coeff",
            nullptr
        );
    }

    dx *= coeff;
    dy *= coeff;
    dz *= coeff;
    return *this;
}

Vector& Vector::operator/=(const double coeff)
{
    if (!std::isfinite(coeff) || coeff == 0)
    {
        throw EXCEPTION_WITH_REASON_CREATE_NEXT_EXCEPTION(
            Utils::SimulatorErrors::InvalidSize,
            "Can't divide vector on infinite coeff or zero",
            nullptr
        );
    }

    dx /= coeff;
    dy /= coeff;
    dz /= coeff;
    return *this;
}

bool Vector::operator==(const Vector& other) const
{
    return dx == other.dx && dy == other.dy && dz == other.dz;
}

Vector operator -(const Vector& self)
{
    return Vector(-self.dx, -self.dy, -self.dz);
}

Vector operator +(const Vector& self, const Vector& other)
{
    Vector tmp = self;
    return tmp += other;
}

Vector operator -(const Vector& self, const Vector& other)
{
    Vector tmp = self;
    return tmp -= other;
}

Vector operator *(const Vector& self, const double coeff)
{
    Vector tmp = self;
    return tmp *= coeff;
}

Vector operator *(const double coeff, const Vector& self)
{
    return self * coeff;
}

Vector operator /(const Vector& self, const double coeff)
{
    Vector tmp = self;
    return tmp /= coeff;
}

double operator ^(const Vector& self, const Vector& other)
{
    return self.dx * other.dx + self.dy * other.dy + self.dz * other.dz;
}

} // namespace Engine
