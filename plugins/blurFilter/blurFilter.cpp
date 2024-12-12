#include "pluginLib/bars/ps_bar.hpp"

#include "blurFilter.hpp"

#include <string>
#include <cassert>

#include "api/api_sfm.hpp"
#include "api/api_photoshop.hpp"
#include "api/api_bar.hpp"
#include "api/api_canvas.hpp"

#include "pluginLib/actions/actions.hpp"
#include "pluginLib/canvas/canvas.hpp"
#include "pluginLib/bars/ps_bar.hpp"
#include "pluginLib/windows/windows.hpp"

#include <iostream>

using namespace ps;
using namespace psapi;
using namespace psapi::sfm;

namespace
{

class BlurFilterButton : public ANamedBarButton 
{
public:
    BlurFilterButton() = default;
    BlurFilterButton(std::unique_ptr<IText> name, std::unique_ptr<IFont> font);

    std::unique_ptr<IAction> createAction(const IRenderWindow* renderWindow, 
                                          const Event& event) override;
    
    bool update(const IRenderWindow* renderWindow, const Event& event);

    void draw(IRenderWindow* renderWindow) override;
};

BlurFilterButton::BlurFilterButton(std::unique_ptr<IText> name, std::unique_ptr<IFont> font)
{
    name_ = std::move(name);
    font_ = std::move(font);
}

std::unique_ptr<IAction> BlurFilterButton::createAction(const IRenderWindow* renderWindow, 
                                                        const Event& event)
{
    return std::make_unique<UpdateCallbackAction<BlurFilterButton>>(*this, renderWindow, event);
}

bool BlurFilterButton::update(const IRenderWindow* renderWindow, const Event& event)
{
    bool updateStateRes = updateState(renderWindow, event);

    if (state_ != State::Released)
        return updateStateRes;

    ICanvas* canvas = static_cast<ICanvas*>(getRootWindow()->getWindowById(kCanvasWindowId));
    assert(canvas);
    
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

void BlurFilterButton::draw(IRenderWindow* renderWindow)
{
    ANamedBarButton::draw(renderWindow);
}

} // namespace anonymous

bool onLoadPlugin()
{
    std::unique_ptr<IText> text = IText::create();
    std::unique_ptr<IFont> font = IFont::create();
    font->loadFromFile("media/fonts/arial.ttf");
    text->setFont(font.get());
    text->setString("Box blur");
    
    auto button = std::make_unique<BlurFilterButton>(std::move(text), std::move(font));

    IWindowContainer* rootWindow = getRootWindow();
    assert(rootWindow);
    auto filterMenu = dynamic_cast<IMenuButton*>(rootWindow->getWindowById(kMenuFilterId));
    assert(filterMenu);

    filterMenu->addMenuItem(std::move(button));

    return true;
}

void onUnloadPlugin()
{
    return;
}

