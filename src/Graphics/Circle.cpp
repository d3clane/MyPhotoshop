#include "Graphics/Circle.hpp"

namespace Graphics
{

Circle::Circle(float radius) : circle_(radius) {}

void Circle::setCenterPos(int x, int y)
{
    double radius = circle_.getRadius();
    circle_.setPosition(x - radius, y - radius);
}

WindowPoint Circle::getCenterPos()
{
    double radius = circle_.getRadius();
    sf::Vector2f pos = circle_.getPosition();
    return {pos.x + radius, pos.y + radius};
}

void Circle::setColor(const Color& color)
{
    sf::Color sfColor(color.red_, color.green_, color.blue_, color.alpha_);
    circle_.setFillColor(sfColor);
}

Color Circle::getColor()
{
    sf::Color sfColor = circle_.getFillColor();
    return {sfColor.r, sfColor.g, sfColor.b, sfColor.a};
}

void Circle::draw(RenderWindow& window)
{
    window.drawCircle(*this);
}

} // namespace Graphics
