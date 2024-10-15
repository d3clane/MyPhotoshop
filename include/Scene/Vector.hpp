#ifndef SCENE_VECTOR_HPP
#define SCENE_VECTOR_HPP

#include "Engine/Vector.hpp"
#include "Scene/Renderable.hpp"

namespace Scene
{

class DrawableVector : public Renderable
{
    Engine::Vector vector_;

public:
    Engine::Point beginPos;

    DrawableVector(const Engine::Vector& v, const Engine::Point& beginPos = {0, 0, 0});

    void draw(Graphics::RenderWindow& renderWindow, const Engine::CoordsSystem& cs) override;
};

} // namespace Scene

#endif // SCENE_VECTOR_HPP