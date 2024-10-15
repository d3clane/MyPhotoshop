#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <SFML/Graphics.hpp>

#include "Graphics/Sprite.hpp"

namespace Graphics
{

class Texture
{
    sf::Texture texture_;

public:
    Texture() = default;
    Texture(unsigned int width, unsigned int height);

    bool loadFromFile(const char* filename);
    
    WindowVector getScale() const;

    static Texture loadTexture(const char* filename);

    friend void Sprite::setTexture(const Texture& texture);
};

} // namespace Graphics

#endif // TEXTURE_HPP