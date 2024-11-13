#include "brush.hpp"
#include <string>
#include <cassert>

#include "api/api_sfm.hpp"
#include "api/api_photoshop.hpp"
#include "api/api_bar.hpp"
#include "api/api_canvas.hpp"

#include "bars/ps_bar.hpp"

#include "interpolation/include/interpolator.hpp"
#include "instrumentsBar/mediator.hpp"
#include "instrumentsBar/instrumentsBar.hpp"

#include <iostream>
#include <memory>

using namespace ps;

namespace ps
{

using namespace psapi;
using namespace psapi::sfm;

class BrushButton : public ASpritedBarButton 
{
public:
    BrushButton() = default;
    BrushButton(std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture);

    virtual bool update(const IRenderWindow* renderWindow, const Event& event) override;

    void setMediator(std::shared_ptr<AFillPropertiesMediator> mediator);
    void setInstrumentsBar(std::unique_ptr<InstrumentsBar> instrumentsBar);

protected:
    Interpolator interpolator_;
    
    std::shared_ptr<AFillPropertiesMediator> mediator_;
    std::unique_ptr<InstrumentsBar> instrumentsBar_;

private:
    void drawPoint(ILayer* layer, const vec2d& point);
};

BrushButton::BrushButton(std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture)
{
    mainSprite_ = std::move(sprite);
    mainTexture_ = std::move(texture);
}

void BrushButton::setMediator(std::shared_ptr<AFillPropertiesMediator> mediator)
{
    mediator_ = mediator;
}

void BrushButton::setInstrumentsBar(std::unique_ptr<InstrumentsBar> instrumentsBar)
{
    instrumentsBar_ = std::move(instrumentsBar_);
}

bool BrushButton::update(const IRenderWindow* renderWindow, const Event& event)
{
    bool updatedState = updateState(renderWindow, event);

    if (state_ != State::Released)
        return updatedState;
    
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

    if (interpolator_.isPossibleToDraw())
    {
        size_t activeLayerIndex = canvas->getActiveLayerIndex();
        ILayer* activeLayer = canvas->getLayer(activeLayerIndex);

        for (double interpolatedPos = 1; interpolatedPos < 2; interpolatedPos += 0.01)
            drawPoint(activeLayer, interpolator_[interpolatedPos]);
        
        interpolator_.popFront();
    }

    vec2i mousePos = canvas->getMousePosition();
    interpolator_.pushBack({mousePos.x, mousePos.y});

    return updatedState;
}

void BrushButton::drawPoint(ILayer* layer, const vec2d& point)
{
    DrawingProperties properties = mediator_->getFillProperties();
    unsigned thickness = properties.thickness;
    int drawingRange = static_cast<int>(thickness + 1) / 2;
    Color color = properties.color;

    for (int i = -drawingRange; i <= drawingRange; ++i)
    {
        for (int j = -drawingRange; j <= drawingRange; ++j)
        {
            vec2d pos = point + vec2d{i, j};
            layer->setPixelOnScreen(vec2i{pos.x, pos.y}, color);
        }
    }
}

} // namespace ps

namespace 
{

std::unique_ptr<ASpritedBarButton> createButton(IBar* toolbar, 
                                                std::shared_ptr<APropertiesMediator> mediator)
                                                //std::unique_ptr<InstrumentsBar> instrumentsBar)
{
    auto buttonSprite  = std::unique_ptr<ISprite>(ISprite::create());
    auto buttonTexture = std::unique_ptr<ITexture>(ITexture::create());

    buttonTexture.get()->loadFromFile("media/textures/paintbrush.png");

    buttonSprite->setTexture(buttonTexture.get());

    auto info = toolbar->getNextChildInfo();
    auto pos = info.pos;
    vec2u size = { static_cast<unsigned int>(info.size.x),  
                   static_cast<unsigned int>(info.size.y) };

    buttonSprite->setPosition(pos.x, pos.y);
    
    auto spriteSize = buttonSprite->getSize();
    buttonSprite->setScale(static_cast<double>(size.x) / spriteSize.x, 
                           static_cast<double>(size.y) / spriteSize.y);

    std::unique_ptr<BrushButton> button{ new ps::BrushButton(std::move(buttonSprite), 
                                                                 std::move(buttonTexture)) };


    button->setPos(pos);
    button->setSize(size);

    button->setMediator(mediator);
    //button->setInstrumentsBar(std::move(instrumentsBar));

    return std::unique_ptr<ASpritedBarButton>(button.release());
}

} // namespace anonymous

bool loadPlugin() // onLoadPlugin
{

    IWindowContainer* rootWindow = getRootWindow();
    assert(rootWindow);
    auto toolbar = static_cast<IBar*>(rootWindow->getWindowById(kToolBarWindowId));
    assert(toolbar);

    auto mediator = std::make_shared<APropertiesMediator>();
    //auto instrumentBar = createCommonInstrumentBar(mediator);
    auto button = createButton(toolbar, mediator);

    toolbar->addWindow(std::move(button));

    return true;
}

void unloadPlugin()
{
    return;
}
