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

// TODO: design fix - menu bar is too big for it's buttons. Up tool options are also too big
IntRect getLayersIntRect()
{
    IntRect rect;
    vec2u size = getScreenSize();

    rect.pos = vec2i{static_cast<int>(static_cast<float>(size.x) * 0.85f), 
                     static_cast<int>(static_cast<float>(size.y) * 0.45f)};

    rect.size = vec2u{static_cast<unsigned>(static_cast<float>(size.x) * 0.15f), 
                      static_cast<unsigned>(static_cast<float>(size.y) * 0.55f)};

    return rect;
}

IntRect getToolOptionsRightIntRect()
{
    IntRect rect;
    vec2u size = getScreenSize();

    rect.pos = vec2i{static_cast<int>(static_cast<float>(size.x) * 0.85f), 
                     static_cast<int>(static_cast<float>(size.y) * 0.08f)};

    rect.size = vec2u{static_cast<unsigned>(static_cast<float>(size.x) * 0.15f), 
                      static_cast<unsigned>(static_cast<float>(size.y) * 0.92f)};

    return rect;
}

IntRect getToolOptionsUpIntRect()
{
    IntRect rect;
    vec2u size = getScreenSize();

    rect.pos = vec2i{0, static_cast<int>(static_cast<float>(size.y) * 0.035f)};
    rect.size = vec2u{size.x, static_cast<unsigned>(static_cast<float>(size.y) * 0.047f)};

    return rect;
}

} // namespace ps