#include "spray.hpp"

#include <string>
#include <cassert>

#include "api/api_sfm.hpp"
#include "api/api_photoshop.hpp"
#include "api/api_bar.hpp"
#include "api/api_canvas.hpp"

#include "bars/ps_bar.hpp"

#include "toolbar/toolbarButton.hpp"
#include "instrumentBar/mediator.hpp"

#include <iostream>

using namespace ps;

namespace
{

using namespace psapi;
using namespace psapi::sfm;

using MediatorType = AFillPropertiesMediator;

class SprayButton : public AInstrumentButton<MediatorType> 
{
public:
    SprayButton() = default;
    SprayButton(std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture);

    bool update(const IRenderWindow* renderWindow, const Event& event) override;

    void draw(IRenderWindow* renderWindow) override;
};

void drawPoint(ICanvas* canvas, std::shared_ptr<MediatorType> mediator);

SprayButton::SprayButton(std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture)
{
    mainSprite_ = std::move(sprite);
    mainTexture_ = std::move(texture);
}

bool SprayButton::update(const IRenderWindow* renderWindow, const Event& event)
{
    bool updatedState = updateState(renderWindow, event);

    instrument_button_functions::updateInstrumentBar(instrumentBar_.get(), state_,
                                                     renderWindow, event);

    if (state_ != State::Released)
        return updatedState;
    
    ICanvas* canvas = static_cast<ICanvas*>(getRootWindow()->getWindowById(kCanvasWindowId));

    if (!canvas)
    {
        std::cerr << "CANVAS NOT FOUND!\n";
        assert(0);
    }

    if (event.type != Event::MouseButtonReleased)
        return true;

    drawPoint(canvas, mediator_);
    
    return updatedState;
}

void SprayButton::draw(IRenderWindow* renderWindow) 
{ 
    ASpritedBarButton::draw(renderWindow, parent_); 

    instrument_button_functions::drawInstrumentBar(instrumentBar_.get(), renderWindow);
}

void drawPoint(ICanvas* canvas, std::shared_ptr<MediatorType> mediator)
{
    size_t activeLayerIndex = canvas->getActiveLayerIndex();
    ILayer* activeLayer = canvas->getLayer(activeLayerIndex);

    vec2i mousePos = canvas->getMousePosition();

    DrawingProperties fillProperties = mediator->fillProperties();

    // TODO: thickness
    Color color = fillProperties.color;
    for (int i = -5; i < 5; ++i)
        for (int j = -5; j < 5; ++j)
            activeLayer->setPixel({mousePos.x + i, mousePos.y + j}, color);
}

} // namespace anonymous

bool loadPlugin()
{
    return instrument_button_functions::instrumentButtonOnLoadPlugin<
        SprayButton, MediatorType>("media/textures/spray.png");
}

void unloadPlugin()
{
    return;
}
