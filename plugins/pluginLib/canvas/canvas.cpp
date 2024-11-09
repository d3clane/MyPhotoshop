#include "canvas.hpp"

#include <cassert>

using namespace psapi;
using namespace psapi::sfm;

namespace ps
{

void copyLayerToLayer(ILayer* dst, const ILayer* src, const vec2u& size)
{
    for (size_t x = 0; x < size.x; ++x)
    {
        for (size_t y = 0; y < size.y; ++y)
        {
            Color color = src->getPixelOnScreen({static_cast<int>(x), static_cast<int>(y)});
            if (color.a == 0)
                continue;
            dst->setPixelOnScreen({static_cast<int>(x), static_cast<int>(y)}, color);
        }
    }
}

void copyImageToLayer(ILayer* dst, const IImage* src, const vec2i& layerPos, const vec2u& size)
{
    assert(src->getSize().x == 1920 && src->getSize().y == 1080);

    for (size_t x = 0; x < size.x; ++x)
    {
        for (size_t y = 0; y < size.y; ++y)
        {
            if (src->getPixel(x, y).a == 0)
                continue;

            dst->setPixelOnScreen(vec2i{x, y} - layerPos, src->getPixel(x, y));
        }
    }
}

} // namespace ps