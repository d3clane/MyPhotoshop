#include "Graphics/PixelsArray.hpp"
#include "Engine/CoordsSystem.hpp"
#include "Engine/Vector.hpp"
#include "Utils/Doubles.hpp"
#include "Utils/Exceptions.hpp"

#include <iostream>

namespace Engine
{

#if 0

namespace 
{

void changeOneArgScale(unsigned int& step, const int delta);

void changeOneArgScale(unsigned int& step, const int delta)
{
    if (delta < 0 && step <= -delta) step = 1;
    else step += delta;
}

} // anonymous namespace

#endif 

using Graphics::WindowLine;

Point& Point::operator+=(const Point& other)
{
    this->x += other.x;
    this->y += other.y;
    this->z += other.z;
    return *this;
}

Point& Point::operator-=(const Point& other)
{
    this->x -= other.x;
    this->y -= other.y;
    this->z -= other.z;
    return *this;
}

Point operator+(const Point& self, const Point& other)
{
    Point selfCpy = self;
    return selfCpy += other;
}

Point operator-(const Point& self, const Point& other)
{
    Point selfCpy = self;
    return selfCpy -= other;
}

CoordsSystem::CoordsSystem(
    const Vector& xBasisVector, const Vector& yBasisVector, const Vector& zBasisVector,
    const Point& center
) : center_(center), 
    xBasisVector_(xBasisVector), 
    yBasisVector_(yBasisVector), 
    zBasisVector_(zBasisVector)
{
    // TODO: assert on linear independence
    if (!(Utils::compare(xBasisVector.lengthSquare(), yBasisVector.lengthSquare()) == 
            Utils::ComparisonResult::Equal &&
          Utils::compare(xBasisVector.lengthSquare(), zBasisVector.lengthSquare()) ==
            Utils::ComparisonResult::Equal))
    {
        throw EXCEPTION_WITH_REASON_CREATE_NEXT_EXCEPTION(
            Utils::SimulatorErrors::InvalidBasisVectorsInCoordsSystem,
            "Basis vectors in coords system have different length",
            nullptr);
    }
}

CoordsSystem::CoordsSystem(const unsigned int stepInPixels, const Point& center) :
    CoordsSystem({stepInPixels, 0, 0}, {0, stepInPixels, 0}, {0, 0, stepInPixels}, center)
{
}

void CoordsSystem::moveCenter(const Vector& delta)
{
    center_ += delta;
}

#if 0
void CoordsSystem::changeScale(const int delta)
{
    changeOneArgScale(stepInPixels_, delta);
    changeOneArgScale(stepInPixels_, delta);
}
#endif 

unsigned int CoordsSystem::getSizeInPixels(const double size) const
{
    if (!std::isfinite(size) || size < 0)
        throw EXCEPTION_WITH_REASON_CREATE_NEXT_EXCEPTION(
            Utils::SimulatorErrors::InvalidSize,
            "size have to be finite and >= 0",
            nullptr
        );
    
    return getVectorInBaseCoordsSystem(Vector{size, 0, 0}).dx;
}

Graphics::WindowVector CoordsSystem::getScaleInPixels(const Vector& scale) const
{
    Vector scaleInPixels = getVectorInBaseCoordsSystem(scale);
    return Graphics::WindowVector{scaleInPixels.dx, scaleInPixels.dy};
}

Vector CoordsSystem::getVectorInBaseCoordsSystem(const Vector& vector) const
{
    return xBasisVector_ * vector.dx + yBasisVector_ * vector.dy + zBasisVector_ * vector.dz;
}

Point CoordsSystem::getPointInBaseCoordsSystem(const Point& point) const
{
    return center_ + getVectorInBaseCoordsSystem(Vector{point});
}

Graphics::WindowPoint CoordsSystem::getPointInWindow(const Point& point) const
{
    Point baseCsPoint = getPointInBaseCoordsSystem(point);

    return Graphics::WindowPoint{baseCsPoint.x, baseCsPoint.y};
}

#if 0
Point CoordsSystem::getPointInCoordsSystem(const Graphics::WindowPoint& point) const
{
    return Point((point.x - center_.x) / stepInPixels_, 
                 (point.y - center_.y) / stepInPixels_, 
                 0);
}
#endif

double getDistance3D(const Point& p1, const Point& p2)
{
    return std::sqrt(getDistance3D(p1, p2));
}

double getDistance2D(const Point& p1, const Point& p2)
{
    return std::sqrt(getDistanceSquare2D(p1, p2));
}

double getDistanceSquare2D(const Point& p1, const Point& p2)
{
    return (p1.x - p2.x) * (p1.x - p2.x) + 
           (p1.y - p2.y) * (p1.y - p2.y);
}

double getDistanceSquare3D(const Point& p1, const Point& p2)
{
    return (p1.x - p2.x) * (p1.x - p2.x) + 
           (p1.y - p2.y) * (p1.y - p2.y) + 
           (p1.z - p2.z) * (p1.z - p2.z);
}

} // namespace Engine