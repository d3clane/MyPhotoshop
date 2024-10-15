#ifndef SCENE_RENDERABLE_HPP
#define SCENE_RENDERABLE_HPP

#include "Graphics/Sprite.hpp"
#include "Engine/CoordsSystem.hpp"

namespace Scene
{

class Renderable
{
public:
    virtual void draw(Graphics::RenderWindow& window, const Engine::CoordsSystem& coordsSystem) = 0;

    virtual ~Renderable() = default;
};

} // namespace Scene

#endif // SCENE_RENDERABLE_HPP