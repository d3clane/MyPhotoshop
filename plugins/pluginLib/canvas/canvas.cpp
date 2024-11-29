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
            Color color = src->getPixel({static_cast<int>(x), static_cast<int>(y)});
            if (color.a == 0)
                continue;
            dst->setPixel({static_cast<int>(x), static_cast<int>(y)}, color);
        }
    }
}

void copyImageToLayer(ILayer* dst, const IImage* src, const vec2i& layerPos, const vec2u& size)
{
    assert(src->getSize().x == 1920 && src->getSize().y == 1080);

    for (unsigned x = 0; x < size.x; ++x)
    {
        for (unsigned y = 0; y < size.y; ++y)
        {
            if (src->getPixel(x, y).a == 0)
                continue;

            dst->setPixel(vec2i{static_cast<int>(x), static_cast<int>(y)} - layerPos, src->getPixel(x, y));
        }
    }
}


std::vector<std::vector<Color>> getLayerScreenIn2D(const ILayer* layer, const vec2u& size)
{
    std::vector<std::vector<Color>> pixels(size.x, std::vector<Color>(size.y));

    for (size_t x = 0; x < size.x; ++x)
        for (size_t y = 0; y < size.y; ++y)
            pixels[x][y] = layer->getPixel(vec2i{static_cast<int>(x), static_cast<int>(y)});


    return pixels;
}

std::vector<Color> getLayerScreenIn1D(const ILayer* layer, const vec2u& size)
{
    std::vector<Color> pixels;

    for (unsigned x = 0; x < size.x; ++x)
        for (unsigned y = 0; y < size.y; ++y)
            pixels.push_back(layer->getPixel(vec2i{static_cast<int>(x), static_cast<int>(y)}));

    return pixels;
}

void copyPixelsToLayer(ILayer* layer, const std::vector<std::vector<Color>>& pixels)
{
    for (size_t x = 0; x < pixels.size(); ++x)
        for (size_t y = 0; y < pixels[x].size(); ++y)
            layer->setPixel(vec2i{static_cast<int>(x), static_cast<int>(y)}, pixels[x][y]);
}

} // namespace ps