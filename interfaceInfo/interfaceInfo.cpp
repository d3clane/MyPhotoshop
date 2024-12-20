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

    rect.pos = vec2i{1463, 64};
    rect.size = vec2u{266, 966};

    return rect;
}

IntRect getToolOptionsUpIntRect()
{
    IntRect rect;
    vec2u size = getScreenSize();

    rect.pos = vec2i{0, 29};
    rect.size = vec2u{size.x, 35};

    return rect;
}

unsigned getCommonTextCharacterSize()
{
    return 18;
}

sfm::IntRect getEmptyDownBarIntRect()
{
    sfm::IntRect rect; 

    rect.pos = {0, 1030};
    rect.size = {1728, 30}; // size out of the screen.

    return rect;
}

psapi::sfm::IntRect getFullCanvasIntRect()
{
    IntRect rect;

    rect.pos = vec2i{72, 64};
    rect.size = vec2u{1378, 966};

    return rect;
}

psapi::sfm::IntRect getCanvasDownScrollBarIntRect()
{
    IntRect rect;

    rect.pos = vec2i{72, 1016};
    rect.size = vec2u{1378, 14};
    // 1374 ans size 14 x 14 for the square there

    return rect;
}

psapi::sfm::IntRect getCanvasRightScrollBarIntRect()
{
    IntRect rect;

    //1460 / 65 + 29 = 1460 / 94
    rect.pos = vec2i{1449, 64};
    rect.size = vec2u{14, 952};

    return rect;
}

psapi::sfm::IntRect getCanvasSquareBetweenScrollIntRect()
{
    IntRect rect;

    rect.size = vec2u{14, 14};
    rect.pos = vec2i{1449, 1016};

    return rect;
}

} // namespace ps