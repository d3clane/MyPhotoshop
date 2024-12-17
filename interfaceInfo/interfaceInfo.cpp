#include "interfaceInfo.hpp"

#include "api/api_photoshop.hpp"

namespace ps
{

using namespace psapi;
using namespace psapi::sfm;

psapi::sfm::Color getCommonBarColor()
{
    return Color{72, 72, 72, 255};
}

const char* getCommonHoverTexturePath()
{
    return "media/textures/ToolbarOnHover.png";
}

const char* getCommonPressTexturePath()
{
    return "media/textures/ToolbarOnPress.png";
}

const char* getCommonReleaseTexturePath()
{
    return "media/textures/ToolbarOnRelease.png";
}

IntRect getLayersIntRect()
{
    IntRect rect;
    vec2i size = getScreenSize();

    rect.pos = vec2i{static_cast<int>(static_cast<float>(size.x) * 0.93f), 
                     static_cast<int>(static_cast<float>(size.y) * 0.45f)};

    rect.size = vec2u{static_cast<unsigned>(static_cast<float>(size.x) * 0.07f), 
                      static_cast<unsigned>(static_cast<float>(size.y) * 0.55f)};

    return rect;
}

IntRect getToolOptionsRightIntRect()
{
    IntRect rect;
    vec2i size = getScreenSize();

    rect.pos = vec2i{static_cast<int>(static_cast<float>(size.x) * 0.93f), 
                     static_cast<int>(static_cast<float>(size.y) * 0.08f)};

    rect.size = vec2u{static_cast<unsigned>(static_cast<float>(size.x) * 0.07f), 
                      static_cast<unsigned>(static_cast<float>(size.y) * 0.92f)};

    return rect;
}

IntRect getUpperToolOptionsIntRect()
{
    IntRect rect;
    vec2i size = getScreenSize();

    rect.pos = vec2i{0, static_cast<int>(static_cast<float>(size.y) * 0.035f)};
    rect.size = vec2u{size.x, static_cast<unsigned>(static_cast<float>(size.y) * 0.045f)};
}

} // namespace ps