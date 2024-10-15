#ifndef GRAPHICS_UPDATABLE_HPP
#define GRAPHICS_UPDATABLE_HPP

#include "Graphics/GraphicsWindow.hpp"
#include "Graphics/Events.hpp"

namespace Graphics
{

class Updatable
{
public:
    virtual bool update(Graphics::RenderWindow& renderWindow, const Graphics::Event& event) = 0;
};

} // namespace Graphics


#endif // GRAPHICS_UPDATABLE_HPP