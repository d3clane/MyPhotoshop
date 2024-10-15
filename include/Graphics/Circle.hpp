#ifndef CIRCLE_HPP
#define CIRCLE_HPP

#include <SFML/Graphics.hpp>
#include "Graphics/GraphicsWindow.hpp"
#include "Graphics/PixelsArray.hpp"
#include "Graphics/Renderable.hpp"

namespace Graphics
{

class Circle : public Renderable
{
    sf::CircleShape circle_;

public:
    Circle(float radius);

    void setCenterPos(int x, int y);
    WindowPoint getCenterPos();

    void setColor(const Color& color);
    Color getColor();

    void draw(RenderWindow& window) override;

    friend void Graphics::RenderWindow::drawCircle(const Circle& circle);
};

} // namespace Graphics

#endif // CIRCLE_HPP