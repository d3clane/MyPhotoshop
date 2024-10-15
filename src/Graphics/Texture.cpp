#include "Graphics/Texture.hpp"

namespace Graphics
{

Texture::Texture(unsigned int width, unsigned int height)
{
    texture_.create(width, height);
}

bool Texture::loadFromFile(const char* filename)
{
    return texture_.loadFromFile(filename);
}

WindowVector Texture::getScale() const
{
    return WindowVector(texture_.getSize().x, texture_.getSize().y);
}

Texture Texture::loadTexture(const char* filename)
{
    Texture texture;
    texture.loadFromFile(filename);
    return texture;
}

} // namespace Graphics