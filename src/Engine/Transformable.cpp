#include "Engine/Transformable.hpp"
#include "Engine/Vector.hpp"

namespace Engine
{

Transformable::Transformable(const Point& topLeft) : transformablePos_(topLeft)
{
}

void Transformable::move(const Vector& delta)
{
    transformablePos_ += delta;
}

void Transformable::pos(const Point& pos)
{
    transformablePos_ = pos;
}

Point Transformable::pos () const
{
    return transformablePos_;
}

} // namespace Engine