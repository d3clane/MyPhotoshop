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

} // namespace ps
