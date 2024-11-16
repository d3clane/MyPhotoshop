#include "bars/ps_bar.hpp"

#include "blurFilter.hpp"

#include <string>
#include <cassert>

#include "api/api_sfm.hpp"
#include "api/api_photoshop.hpp"
#include "api/api_bar.hpp"
#include "api/api_canvas.hpp"
#include "canvas/canvas.hpp"

#include "bars/ps_bar.hpp"
#include "windows/windows.hpp"

#include <iostream>

using namespace ps;
using namespace psapi;
using namespace psapi::sfm;

namespace
{

class BlurFilterButton : public ASpritedBarButton 
{
public:
    BlurFilterButton() = default;
    BlurFilterButton(std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture);

    bool update(const IRenderWindow* renderWindow, const Event& event) override;
};

BlurFilterButton::BlurFilterButton(std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture)
{
    mainSprite_ = std::move(sprite);
    mainTexture_ = std::move(texture);
}

bool BlurFilterButton::update(const IRenderWindow* renderWindow, const Event& event)
{
    bool updateStateRes = updateState(renderWindow, event);

    if (state_ != State::Released)
        return updateStateRes;

    ICanvas* canvas = static_cast<ICanvas*>(getRootWindow()->getWindowById(kCanvasWindowId));
    if (!canvas)
    {
        std::cerr << "CANVAS NOT FOUND!\n";
        assert(0);
    }
    
    size_t activeLayerIndex = canvas->getActiveLayerIndex();
    ILayer* activeLayer = canvas->getLayer(activeLayerIndex);

    vec2u canvasSize = canvas->getSize();

    std::vector<std::vector<Color>> colors(canvasSize.x, std::vector<Color>(canvasSize.y, Color{0, 0, 0, 0}));

    for (int x = 0; x < static_cast<int>(canvasSize.x); ++x)
    {
        for (int y = 0; y < static_cast<int>(canvasSize.y); ++y)
        {
            int r = 0, g = 0, b = 0, a = 0;
            Color newColor = {0, 0, 0, 0};

            for (int deltaX = -1; deltaX <= 1; ++deltaX)
            {
                for (int deltaY = -1; deltaY <= 1; ++deltaY)
                {
                    Color pixelColor = activeLayer->getPixel(vec2i{x + deltaX, y + deltaY});
                    r += pixelColor.r; g += pixelColor.g; b += pixelColor.b; a += pixelColor.a;
                }
            }

            r /= 9; g /= 9; b /= 9; a /= 9;

#if 0
            if (r != 0 && g != 0)
            {
                std::cout << pr << " " << pg << " " << pb << " " << pa << "\n";
                std::cout << r << " " << g << " " << b << " " << a << "\n";
            }
            r = pr + (pr - r) * 2; g = pg + (pg - g) * 2; b = pb + (pb - b) * 2; a = s;
            r = std::clamp(r, 0, 255); g = std::clamp(g, 0, 255); b = std::clamp(b, 0, 255); a = std::clamp(a, 0, 255);
#endif

            newColor = {static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b), static_cast<uint8_t>(a)};

            colors[static_cast<size_t>(x)][static_cast<size_t>(y)] = newColor;
        }
    }

    for (size_t x = 0; x < canvasSize.x; ++x)
    {
        for (size_t y = 0; y < canvasSize.y; ++y)
        {
            activeLayer->setPixel(vec2i{static_cast<int>(x), static_cast<int>(y)}, colors[x][y]);
        }
    }
    
    state_ = State::Normal;

    return true;
}

}

bool loadPlugin() // onLoadPlugin
{
    auto buttonSprite  = ISprite::create();
    auto buttonTexture = ITexture::create();

    buttonTexture.get()->loadFromFile("media/textures/paintbrush.png");

    buttonSprite->setTexture(buttonTexture.get());

    IWindowContainer* rootWindow = getRootWindow();
    assert(rootWindow);
    auto toolBar = static_cast<IBar*>(rootWindow->getWindowById(kToolBarWindowId));
    assert(toolBar);

    auto info = toolBar->getNextChildInfo();
    vec2i pos = info.pos;
    vec2u size = { static_cast<unsigned int>(info.size.x),  
                   static_cast<unsigned int>(info.size.y) };

    buttonSprite->setPosition(pos.x, pos.y);
    
    vec2u spriteSize = buttonSprite->getSize();
    buttonSprite->setScale(static_cast<float>(size.x) / static_cast<float>(spriteSize.x), 
                           static_cast<float>(size.y) / static_cast<float>(spriteSize.y));
    std::unique_ptr<ps::ASpritedBarButton> button{ new BlurFilterButton(std::move(buttonSprite), std::move(buttonTexture)) };

    button->setPos(pos);
    button->setSize(size);

    assert(rootWindow->getWindowById(kToolBarWindowId));
    
    static_cast<IBar*>(rootWindow->getWindowById(kToolBarWindowId))->
        addWindow(std::unique_ptr<ps::IBarButton>(button.release()));

    return true;
}

void unloadPlugin()
{
    return;
}
