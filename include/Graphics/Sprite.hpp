#ifndef SPRITE_HPP
#define SPRITE_HPP

#include <SFML/Graphics.hpp>
#include <chrono>

#include "Graphics/GraphicsWindow.hpp"
#include "Graphics/Renderable.hpp"

namespace Graphics
{

struct Color;
class Texture;

class Sprite : public Renderable
{
    sf::Sprite sprite_;

public:

    void setTexture (const Texture& texture);
    void setPosition(const WindowPoint& position);
    void scale      (const float scaleX, const float scaleY);

    void scaleInPixels(const WindowVector& scaleInPixels);

    WindowVector getScaleInPixels() const;

    void  setColor(const Color& color);
    Color getColor() const;

    void draw(RenderWindow& window) override;

    friend void RenderWindow::drawSprite(const Sprite& sprite);
};


} // namespace Graphics

#endif // SPRITE_HPP