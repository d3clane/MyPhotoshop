#include "brush.hpp"
#include <string>
#include <cassert>

#include "api/api_sfm.hpp"
#include "api/api_photoshop.hpp"
#include "api/api_bar.hpp"
#include "api/api_canvas.hpp"

#include "pluginLib/bars/ps_bar.hpp"

#include "interpolation/include/interpolator.hpp"
#include "instrumentBar/mediator.hpp"
#include "instrumentBar/instrumentBar.hpp"

#include "toolbar/toolbarButton.hpp"

#include <iostream>
#include <memory>

using namespace ps;

using MediatorType = AFillPropertiesMediator;

namespace
{

using namespace psapi;
using namespace psapi::sfm;

class BrushButton : public AInstrumentButton<MediatorType> 
{
public:
    BrushButton() = default;
    BrushButton(std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture);

    bool update(const IRenderWindow* renderWindow, const Event& event) override;
    void draw(IRenderWindow* renderWindow) override;

protected:
    Interpolator interpolator_;
};

void drawPoint(ILayer* layer, const vec2i& point, std::shared_ptr<MediatorType> mediator);
bool drawTrace(ICanvas* canvas, std::shared_ptr<MediatorType> mediator, const Interpolator& interpolator);

BrushButton::BrushButton(std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture)
{
    mainSprite_ = std::move(sprite);
    mainTexture_ = std::move(texture);
}

bool BrushButton::update(const IRenderWindow* renderWindow, const Event& event)
{
    bool updatedState = updateState(renderWindow, event);

    instrument_button_functions::updateInstrumentBar(instrumentBar_.get(), state_, renderWindow, event);

    if (state_ != State::Released)
        return updatedState;
    
    // TODO: may be really slow
    ICanvas* canvas = static_cast<ICanvas*>(getRootWindow()->getWindowById(kCanvasWindowId));

    if (!canvas)
    {
        std::cerr << "CANVAS NOT FOUND!\n";
        assert(0);
    }

    if (!canvas->isPressed())
    {
        interpolator_.clear();
        return true;
    }

    if (drawTrace(canvas, mediator_, interpolator_))
        interpolator_.popFront();

    vec2i mousePos = canvas->getMousePosition();
    interpolator_.pushBack({mousePos.x, mousePos.y});

    return updatedState;
}

void BrushButton::draw(IRenderWindow* renderWindow)
{
    if (!isActive_)
        return;

    ASpritedBarButton::draw(renderWindow, parent_);

    instrument_button_functions::drawInstrumentBar(instrumentBar_.get(), renderWindow);
}

bool drawTrace(ICanvas* canvas, std::shared_ptr<MediatorType> mediator, const Interpolator& interpolator)
{
    if (!interpolator.isPossibleToDraw())
        return false;

    size_t activeLayerIndex = canvas->getActiveLayerIndex();
    ILayer* activeLayer = canvas->getLayer(activeLayerIndex);

    for (double interpolatedPos = 1; interpolatedPos < 2; interpolatedPos += 0.01)
    {
        vec2d interpolatedPoint = interpolator[interpolatedPos];
        drawPoint(activeLayer, 
                  vec2i{static_cast<int>(interpolatedPoint.x), static_cast<int>(interpolatedPoint.y)},
                  mediator);
    }

    return true;
}

void drawPoint(ILayer* layer, const vec2i& point, std::shared_ptr<MediatorType> mediator)
{
    DrawingProperties properties = mediator->fillProperties();
    unsigned thickness = properties.thickness;
    int drawingRange = static_cast<int>(thickness + 1) / 2;
    Color color = properties.color;

    for (int i = -drawingRange; i <= drawingRange; ++i)
    {
        for (int j = -drawingRange; j <= drawingRange; ++j)
        {
            vec2i pos = point + vec2i{i, j};
            layer->setPixel(vec2i{pos.x, pos.y}, color);
        }
    }
}

} // namespace anonymous

bool loadPlugin() // onLoadPlugin
{
    return instrument_button_functions::instrumentButtonOnLoadPlugin<
        BrushButton, MediatorType>("media/textures/paintbrush.png");
}

void unloadPlugin()
{
    return;
}
