#include "filters.hpp"

#include <cassert>

namespace ps
{

std::vector<std::vector<Color>> getNegative(const std::vector<std::vector<Color>>& pixels)
{
    std::vector<std::vector<Color>> negative(pixels.size(), std::vector<Color>(pixels[0].size()));

    for (size_t x = 0; x < pixels.size(); ++x)
    {
        assert(pixels[x].size() == pixels[0].size());

        for (size_t y = 0; y < pixels[0].size(); ++y)
        {
            negative[x][y] = {255 - pixels[x][y].r, 
                              255 - pixels[x][y].g, 
                              255 - pixels[x][y].b, 
                                    pixels[x][y].a};
        }
    }

    return negative;
}

std::vector<std::vector<Color>> getBasRelief(const std::vector<std::vector<Color>>& pixels, 
                                             const std::vector<std::vector<Color>>& negative)
{
    assert(pixels.size() == negative.size());

    std::vector<std::vector<Color>> basRelief(pixels.size(), std::vector<Color>(pixels[0].size()));

    unsigned xSize = static_cast<unsigned>(pixels.size());
    unsigned ySize = xSize > 0 ? static_cast<unsigned>(pixels[0].size()) : 0;

    for (unsigned x = 0; x < xSize; ++x)
    {
        for (unsigned y = 0; y < ySize; ++y)
        {
            assert(pixels[x].size() == pixels[0].size());
            assert(pixels[x].size() == negative[x].size());

            Color color = pixels[std::clamp(x + 1, 0u, xSize - 1)]
                                [std::clamp(y + 1, 0u, ySize - 1)];

            Color negColor = negative[x][y];
            Color newColor = Color{(negColor.r + color.r) / 2, 
                                   (negColor.g + color.g) / 2, 
                                   (negColor.b + color.b) / 2, 
                                                 color.a};
            
            basRelief[x][y] = newColor;
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
    assert(x0 < static_cast<int>(pixels[(size_t)x0].size()));

    int range = 1;

    int resultR = 0, resultB = 0, resultG = 0;

    int divider = 0;
    for (int y = y0 - range; y < y0 + range; ++y)
    {
        for (int x = x0 - range; x < x0 + range; ++x)
        {
            if (!valueInBoundaries(y, -1, static_cast<int>(pixels           .size())) || 
                !valueInBoundaries(x, -1, static_cast<int>(pixels[(size_t)y].size())))
                continue;

            divider++;

            resultR += static_cast<int>(pixels[(size_t)y][(size_t)x].r);
            resultB += static_cast<int>(pixels[(size_t)y][(size_t)x].b);
            resultG += static_cast<int>(pixels[(size_t)y][(size_t)x].g);
        }
    }

    assert(divider > 0);

    return Color{static_cast<uint8_t>(resultR / divider), 
                 static_cast<uint8_t>(resultG / divider), 
                 static_cast<uint8_t>(resultB / divider), 
                 pixels[(size_t)y0][(size_t)x0].a};
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
