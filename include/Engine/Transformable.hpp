#ifndef ENGINE_TRANSFORMABLE_HPP
#define ENGINE_TRANSFORMABLE_HPP

#include "Engine/CoordsSystem.hpp"

namespace Engine
{

class Vector;

class Transformable
{
protected:
    Point transformablePos_;

public:
    Transformable(const Point& topLeft);

    virtual void  move(const Vector& delta);
    virtual void  pos (const Point&  pos  );
    virtual Point pos () const;

    virtual ~Transformable() = default;
};

} // namespace Engine

#endif // ENGINE_TRANSFORMABLE_HPP