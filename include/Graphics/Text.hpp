#ifndef GRAPHICS_TEXT_HPP
#define GRAPHICS_TEXT_HPP

#include <SFML/Graphics.hpp>

#include "Graphics/GraphicsWindow.hpp"
#include "Graphics/Renderable.hpp"

namespace Graphics
{

class Font;

class Text : public Renderable
{
    sf::Text text_;

public:
    void setFont    (const Font& font);
    void setString  (const std::string& string);
    void setPosition(const WindowPoint& position);

    void draw(Graphics::RenderWindow& renderWindow) override;
    
    friend void RenderWindow::drawText(const Text& text);
};

} // namespace Graphics

#endif // GRAPHICS_TEXT_HPP