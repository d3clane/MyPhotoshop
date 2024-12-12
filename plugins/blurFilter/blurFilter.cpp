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

#include "pluginLib/filters/filters.hpp"

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

    std::vector<std::vector<Color>> pixels = getLayerScreenIn2D(activeLayer, canvasSize);
    std::vector<std::vector<Color>> blured = getBoxBlured(pixels);
    
    copyPixelsToLayer(activeLayer, blured);
    
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

