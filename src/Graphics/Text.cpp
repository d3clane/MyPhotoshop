#include "Graphics/Text.hpp"
#include "Graphics/Font.hpp"

namespace Graphics
{

void Text::setFont(const Font& font)
{
    text_.setFont(font.font_);
}

void Text::setString(const std::string& string)
{
    text_.setString(string);
}

void Text::setPosition(const WindowPoint& position)
{
    text_.setPosition(position.x, position.y);
}

void Text::draw(Graphics::RenderWindow& renderWindow)
{
    renderWindow.drawText(*this);
}


} // namespace Graphics