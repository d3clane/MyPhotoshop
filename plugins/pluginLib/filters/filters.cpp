#include "filters.hpp"

#include <cassert>

namespace ps
{

std::vector<std::vector<Color>> getNegative(const std::vector<std::vector<Color>>& pixels)
{
    std::vector<std::vector<Color>> negative(pixels.size(), std::vector<Color>(pixels[0].size()));

    for (size_t y = 0; y < pixels.size(); ++y)
    {
        assert(pixels[y].size() == pixels[0].size());

        for (size_t x = 0; x < pixels[0].size(); ++x)
        {
            negative[y][x] = {255 - pixels[y][x].r, 
                              255 - pixels[y][x].g, 
                              255 - pixels[y][x].b, 
                                    pixels[y][x].a};
        }
    }

    return negative;
}

std::vector<std::vector<Color>> getBasRelief(const std::vector<std::vector<Color>>& pixels, 
                                             const std::vector<std::vector<Color>>& negative)
{
    assert(pixels.size() == negative.size());

    std::vector<std::vector<Color>> basRelief(pixels.size(), std::vector<Color>(pixels[0].size()));

    unsigned ySize = static_cast<unsigned>(pixels.size());
    unsigned xSize = ySize > 0 ? static_cast<unsigned>(pixels[0].size()) : 0;

    for (unsigned y = 0; y < ySize; ++y)
    {
        for (unsigned x = 0; x < xSize; ++x)
        {
            assert(pixels[y].size() == pixels[0].size());
            assert(pixels[y].size() == negative[y].size());

            Color color = pixels[std::clamp(y + 1, 0u, ySize - 1)]
                                [std::clamp(x + 1, 0u, xSize - 1)];

            Color negColor = negative[y][x];
            Color newColor = Color{(negColor.r + color.r) / 2, 
                                   (negColor.g + color.g) / 2, 
                                   (negColor.b + color.b) / 2, 
                                                 color.a};
            
            basRelief[y][x] = newColor;
        }
    }

    return basRelief;    
}

// excluding boundaries
static bool valueInBoundaries(int value, int boundaryBottom, int boundaryTop)
{
    return boundaryBottom < value && value < boundaryTop;
}

static Color boxBlurPixel(const std::vector<std::vector<Color>>& pixels, int x0, int y0)
{
    assert(x0 >= 0);
    assert(y0 >= 0);
    assert(y0 < static_cast<int>(pixels.size()));
    assert(x0 < static_cast<int>(pixels[(size_t)y0].size()));

    int range = 1;
    int resultR = 0, resultB = 0, resultG = 0, resultA = 0;
    int divider = 0;

    for (int y = y0 - range; y <= y0 + range; ++y)
    {
        for (int x = x0 - range; x <= x0 + range; ++x)
        {
            if (!valueInBoundaries(y, -1, static_cast<int>(pixels           .size())) || 
                !valueInBoundaries(x, -1, static_cast<int>(pixels[(size_t)y].size())))
                continue;

            divider++;

            resultR += static_cast<int>(pixels[(size_t)y][(size_t)x].r);
            resultB += static_cast<int>(pixels[(size_t)y][(size_t)x].b);
            resultG += static_cast<int>(pixels[(size_t)y][(size_t)x].g);
            resultA += static_cast<int>(pixels[(size_t)y][(size_t)x].a);
        }
    }

    assert(divider > 0);

    return Color{static_cast<uint8_t>(resultR / divider), 
                 static_cast<uint8_t>(resultG / divider), 
                 static_cast<uint8_t>(resultB / divider), 
                 static_cast<uint8_t>(resultA / divider)};
}

std::vector<std::vector<Color>> getBoxBlured(const std::vector<std::vector<Color>>& pixels)
{
    std::vector<std::vector<Color>> result = pixels;

    for (size_t y = 0; y < pixels.size(); ++y)
    {
        for (size_t x = 0; x < pixels[y].size(); ++x)    
        {
            result[y][x] = boxBlurPixel(pixels, static_cast<int>(x), static_cast<int>(y));
        }
    }

    return result;
}

} // namespace ps
