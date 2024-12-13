#include "negFilter.hpp"
#include "pluginLib/bars/ps_bar.hpp"

#include "negFilter.hpp"

#include <string>
#include <cassert>

#include "api/api_sfm.hpp"
#include "api/api_photoshop.hpp"
#include "api/api_bar.hpp"
#include "api/api_canvas.hpp"

#include "pluginLib/canvas/canvas.hpp"
#include "pluginLib/bars/ps_bar.hpp"
#include "pluginLib/windows/windows.hpp"
#include "pluginLib/filters/filters.hpp"
#include "pluginLib/actions/actions.hpp"
#include "pluginLib/bars/menu.hpp"

#include <iostream>

using namespace ps;
using namespace psapi;
using namespace psapi::sfm;

namespace
{

class NegativeFilterButton : public ANamedBarButton 
{
public:
    NegativeFilterButton(std::unique_ptr<IText> text, std::unique_ptr<IFont> font);

    std::unique_ptr<IAction> createAction(const IRenderWindow* renderWindow, const Event& event) override;
    bool update(const IRenderWindow* renderWindow, const Event& event);
};

NegativeFilterButton::NegativeFilterButton(std::unique_ptr<IText> text, std::unique_ptr<IFont> font)
{
    name_ = std::move(text);
    font_ = std::move(font);
}

std::unique_ptr<IAction> NegativeFilterButton::createAction(const IRenderWindow* renderWindow, 
                                                            const Event& event)
{
    return std::make_unique<UpdateCallbackAction<NegativeFilterButton>>(*this, renderWindow, event);
}

bool NegativeFilterButton::update(const IRenderWindow* renderWindow, const Event& event)
{
    bool updateStateRes = updateState(renderWindow, event);

    if (state_ != State::Released)
        return updateStateRes;

    ICanvas* canvas = static_cast<ICanvas*>(getRootWindow()->getWindowById(kCanvasWindowId));
    assert(canvas);
    
    size_t activeLayerIndex = canvas->getActiveLayerIndex();
    ILayer* activeLayer = canvas->getLayer(activeLayerIndex);

    vec2u canvasSize = canvas->getSize();

    std::vector<std::vector<Color>> pixels = getLayerScreenIn2D(activeLayer, canvasSize);
    std::vector<std::vector<Color>> negative = getNegative(pixels);
    
    std::vector<std::vector<Color>> basRelief = getBasRelief(pixels, negative);
    copyPixelsToLayer(activeLayer, negative);
    
    state_ = State::Normal;

    return true;
}

}

bool onLoadPlugin()
{
    std::unique_ptr<IText> text = IText::create();
    std::unique_ptr<IFont> font = IFont::create();
    font->loadFromFile("media/fonts/arial.ttf");
    text->setFont(font.get());
    text->setString("Negative");
    
    auto button = std::make_unique<NegativeFilterButton>(std::move(text), std::move(font));

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
