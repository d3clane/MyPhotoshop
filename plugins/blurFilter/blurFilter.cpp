#include "pluginLib/bars/ps_bar.hpp"

#include "blurFilter.hpp"
#include "pluginLib/filters/slider.hpp"

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

#include "pluginLib/filters/filterWindows.hpp"

#include <iostream>

using namespace ps;
using namespace psapi;
using namespace psapi::sfm;

namespace
{

class BlurFilterButton : public ANamedBarButton 
{
public:
    BlurFilterButton(std::unique_ptr<IText> name, std::unique_ptr<IFont> font);

    std::unique_ptr<IAction> createAction(const IRenderWindow* renderWindow, 
                                          const Event& event) override;
    
    bool update(const IRenderWindow* renderWindow, const Event& event);
    void draw(IRenderWindow* renderWindow) override;

private:
    std::vector<std::vector<Color>> beginLayer_;

    std::unique_ptr<FilterWindow> filterWindow_;
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
    {
        if (filterWindow_)
        {
            filterWindow_->close();
            filterWindow_.reset();
        }

        return updateStateRes;
    }

    ICanvas* canvas = static_cast<ICanvas*>(getRootWindow()->getWindowById(kCanvasWindowId));
    assert(canvas);
    
    size_t activeLayerIndex = canvas->getActiveLayerIndex();
    ILayer* activeLayer = canvas->getLayer(activeLayerIndex);

    vec2u canvasSize = canvas->getSize();

    if (updateStateRes)
    {
        beginLayer_ = getLayerScreenIn2D(activeLayer, canvasSize);
        filterWindow_ = createSimpleFilterWindow("Box Blur");
    }

    assert(filterWindow_);

    AActionController* actionController = getActionController();

    NamedSlider* radiusSlider = dynamic_cast<NamedSlider*>(filterWindow_->getWindowById(kRadiusSliderId));
    
    if (radiusSlider)
    {
        int radius = static_cast<int>(radiusSlider->getCurrentValue());

        std::vector<std::vector<Color>> blured = getBoxBlured(beginLayer_, radius, radius);
        
        copyPixelsToLayer(activeLayer, blured);
    }

    if (!actionController->execute(filterWindow_->createAction(renderWindow, event)))
    {
        state_ = State::Normal;
        return false;
    }

    return true;
}

void BlurFilterButton::draw(IRenderWindow* renderWindow)
{
    ANamedBarButton::draw(renderWindow);

    if (filterWindow_)
        filterWindow_->draw(renderWindow);
}
} // namespace anonymous

bool onLoadPlugin()
{
    std::unique_ptr<IText> text = IText::create();
    std::unique_ptr<IFont> font = IFont::create();
    font->loadFromFile("media/fonts/arial.ttf");
    text->setFont(font.get());
    text->setString("Box Blur");
    
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

