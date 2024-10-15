#ifndef ENGINE_COORDS_SYSTEM_HPP
#define ENGINE_COORDS_SYSTEM_HPP

#include "Graphics/GraphicsWindow.hpp"
#include "Engine/Vector.hpp"

namespace Engine
{

struct Point
{
    double x, y, z;

    Point(double x, double y, double z) : x(x), y(y), z(z) {}
    //Point(Graphics::WindowPoint windowPoint) : x(windowPoint.x), y(windowPoint.y), z(0) {}

    Point& operator+=(const Point& other);
    Point& operator-=(const Point& other);
};

Point operator+(const Point& self, const Point& other);
Point operator-(const Point& self, const Point& other);

class CoordsSystem
{
    Point center_;
    Vector xBasisVector_, yBasisVector_, zBasisVector_;

public:
    CoordsSystem(const unsigned int stepInPixels, const Point&);

    CoordsSystem(
        const Vector& xBasisVector, const Vector& yBasisVector, const Vector& zBasisVector,
        const Point& center
    );

    void moveCenter (const Vector& deltaPixels);
    
#if 0
    void changeScale(const int delta);
#endif

    unsigned int getSizeInPixels(const double size) const;
    Graphics::WindowVector getScaleInPixels(const Vector& scale) const;

    Vector getVectorInBaseCoordsSystem(const Vector& vector) const;
    Point  getPointInBaseCoordsSystem (const Point& point  ) const;

    Graphics::WindowPoint getPointInWindow(const Point& point) const;

#if 0
    Point getPointInCoordsSystem(const Graphics::WindowPoint& point) const;
#endif
};

double getDistanceSquare3D(const Point& p1, const Point& p2);
double getDistanceSquare2D(const Point& p1, const Point& p2);
double getDistance3D      (const Point& p1, const Point& p2);
double getDistance2D      (const Point& p1, const Point& p2);

} // namespace Engine

#endif // ENGINE_COORDS_SYSTEM_HPP