#include "sfmHelpful.hpp"

#include <string>
#include <cassert>

namespace ps
{

using namespace psapi;
using namespace psapi::sfm;

SpriteInfo createSprite(const vec2u& size, const char* filename)
{
    SpriteInfo info;
    info.sprite = ISprite::create();
    info.texture = ITexture::create();

    bool loadResult = info.texture->loadFromFile(filename);
    assert(loadResult);
    info.sprite->setTexture(info.texture.get());

    info.sprite->setScale(static_cast<float>(size.x) / static_cast<float>(info.texture->getSize().x), 
                          static_cast<float>(size.y) / static_cast<float>(info.texture->getSize().y));

    return info;
}

std::unique_ptr<IRectangleShape> createShape(Color color, const vec2u& size)
{
    auto shape = IRectangleShape::create(size.x, size.y);

    shape->setFillColor(color);
    shape->setOutlineThickness(0);

    return shape;
}

bool operator==(const Color& a, const Color& b)
{
    return a.a == b.a && a.r == b.r && a.g == b.g && a.b == b.b;
}

} // namespace ps