#include "ellipse.hpp"

#include <string>
#include <cassert>

#include "api/api_sfm.hpp"
#include "api/api_photoshop.hpp"
#include "api/api_bar.hpp"
#include "api/api_canvas.hpp"
#include "canvas/canvas.hpp"

#include "bars/ps_bar.hpp"
#include "windows/windows.hpp"

#include "instrumentBar/mediator.hpp"
#include "instrumentBar/instrumentBar.hpp"

#include "toolbar/toolbarButton.hpp"

#include <iostream>

using namespace ps;
using namespace psapi;
using namespace psapi::sfm;

namespace
{

using MediatorType = APropertiesMediator;

struct EllipseBounds
{
    vec2i topLeft;
    vec2i bottomRight;
};

EllipseBounds getEllipseBounds(const vec2i& topLeftNow, const vec2i& bottomRightNow)
{
    vec2i topLeft = topLeftNow;
    vec2i bottomRight = bottomRightNow;

    if (topLeftNow.x > bottomRightNow.x)
    {
        topLeft.x = bottomRightNow.x;
        bottomRight.x = topLeftNow.x;
    }

    if (topLeftNow.y > bottomRightNow.y)
    {
        topLeft.y = bottomRightNow.y;
        bottomRight.y = topLeftNow.y;
    }

    return {topLeft, bottomRight};
}

class EllipseButton : public AInstrumentButton<MediatorType>
{
public:
    EllipseButton() = default;
    EllipseButton(std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture);

    bool update(const IRenderWindow* renderWindow, const Event& event) override;
    void draw(IRenderWindow* renderWindow) override;

private:
    bool canvasIsAlreadyPressed_ = false;
    vec2i beginEllipsePos_;

    std::unique_ptr<IEllipseShape> ellipse_; // the same crutch
};

void copyEllipseToLayer(ILayer* layer, const IEllipseShape* ellipse, const vec2i& canvasPos)
{
    const IImage* image = ellipse->getImage();
    if (!image)
        return;
    
    copyImageToLayer(layer, image, canvasPos, image->getSize());
}

std::unique_ptr<IEllipseShape> createEllipseShape(const vec2i& beginEllipsePos, const ICanvas* canvas,
                                                  std::shared_ptr<MediatorType> mediator)
{
    vec2i canvasPos  = canvas->getPos();
    vec2i mousePos = canvas->getMousePosition() + canvasPos;

    EllipseBounds bounds = getEllipseBounds(beginEllipsePos, mousePos);

    vec2i topLeft     = bounds.topLeft;
    vec2i bottomRight = bounds.bottomRight;
    
    assert(bottomRight.y >= topLeft.y);
    assert(bottomRight.x >= topLeft.x);
    const unsigned ellipseYSize = static_cast<unsigned>(bottomRight.y - topLeft.y);
    const unsigned ellipseXSize = static_cast<unsigned>(bottomRight.x - topLeft.x);

    std::unique_ptr<IEllipseShape> ellipse = IEllipseShape::create(ellipseXSize, ellipseYSize);
    assert(ellipse);

    DrawingProperties fillProperties = mediator->fillProperties();
    Color color = fillProperties.color;

    ellipse->setFillColor(color);
    ellipse->setOutlineThickness(0);
    ellipse->setPosition(topLeft);

    return ellipse;
}

EllipseButton::EllipseButton(std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture)
{
    mainSprite_ = std::move(sprite);
    mainTexture_ = std::move(texture);
}

bool EllipseButton::update(const IRenderWindow* renderWindow, const Event& event)
{
    bool updateStateRes = updateState(renderWindow, event);

    instrument_button_functions::updateInstrumentBar(instrumentBar_.get(), state_,
                                                    renderWindow, event);

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

    vec2i canvasPos = canvas->getPos();

    if (!canvas->isPressed() && canvasIsAlreadyPressed_)
    {
        copyEllipseToLayer(activeLayer, ellipse_.get(), canvasPos);
        ellipse_.reset();
    }
    
    if (!canvas->isPressed())
    {
        canvasIsAlreadyPressed_ = false;
        return updateStateRes;
    }

    if (!canvasIsAlreadyPressed_)
    {
        beginEllipsePos_ = canvas->getMousePosition() + canvasPos;
        canvasIsAlreadyPressed_ = true;
    }

    ellipse_ = createEllipseShape(beginEllipsePos_, canvas, mediator_);

    if (ellipse_)
        const_cast<IRenderWindow*>(renderWindow)->draw(ellipse_.get()); // crutch

    return true;
}

void EllipseButton::draw(IRenderWindow* renderWindow)
{
    ASpritedBarButton::draw(renderWindow, parent_);

    if (ellipse_)
        renderWindow->draw(ellipse_.get());
    
    instrument_button_functions::drawInstrumentBar(instrumentBar_.get(), renderWindow);
}

} // namespace anonymous

bool loadPlugin() // onLoadPlugin
{
    return instrument_button_functions::instrumentButtonOnLoadPlugin<
        EllipseButton, MediatorType>("media/textures/paintbrush.png");
}

void unloadPlugin()
{
    return;
}
