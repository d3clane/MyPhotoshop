#ifndef GRAPHICS_RENDERABLE_HPP
#define GRAPHICS_RENDERABLE_HPP

#include <SFML/Graphics.hpp>

namespace Graphics
{

class RenderWindow;

class Renderable
{
public:
    virtual void draw(Graphics::RenderWindow& renderWindow) = 0;    

    virtual ~Renderable() = default;
};

} // namespace Graphics

#endif