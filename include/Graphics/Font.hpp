#ifndef GRAPHICS_FONT_HPP
#define GRAPHICS_FONT_HPP

#include <SFML/Graphics.hpp>

#include "Graphics/Text.hpp"

namespace Graphics
{

class Font final
{
    sf::Font font_;

public:
    bool loadFromFile(const std::string& fileName);

    friend void Text::setFont(const Font& font);
};

} // namespace Graphics

#endif // GRAPHICS_FONT_HPP