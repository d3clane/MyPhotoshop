#include "ellipse.hpp"

#include <string>
#include <cassert>

#include "api/api_sfm.hpp"
#include "api/api_photoshop.hpp"
#include "api/api_bar.hpp"
#include "api/api_canvas.hpp"
#include "canvas/canvas.hpp"

#include "bars/ps_bar.hpp"

#include <iostream>

namespace ps
{

using namespace psapi;
using namespace psapi::sfm;

namespace 
{

struct EllipseBounds
{
    vec2i topLeft;
    vec2i bottomRight;
};

EllipseBounds getEllipseBounds(vec2i& topLeftNow, const vec2i& bottomRightNow)
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

} // namespace anonymous

class EllipseButton : public ABarButton 
{
public:
    EllipseButton() = default;
    EllipseButton(std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture);

    virtual bool update(const IRenderWindow* renderWindow, const Event& event) override;

private:
    bool canvasIsAlreadyPressed_ = false;

    vec2i beginEllipsePos_;
};

EllipseButton::EllipseButton(std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture)
{
    mainSprite_ = std::move(sprite);
    mainTexture_ = std::move(texture);
}

bool EllipseButton::update(const IRenderWindow* renderWindow, const Event& event)
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
    ILayer* tempLayer   = canvas->getTempLayer();

    if (!canvas->isPressed() && canvasIsAlreadyPressed_)
        copyLayerToLayer(activeLayer, tempLayer, canvas->getSize());
    
    canvas->cleanTempLayer();

    if (!canvas->isPressed())
    {
        canvasIsAlreadyPressed_ = false;
        return updateStateRes;
    }

    vec2i canvasPos = canvas->getPos();

    if (!canvasIsAlreadyPressed_)
    {
        beginEllipsePos_ = canvas->getMousePosition() + canvasPos;
        canvasIsAlreadyPressed_ = true;
    }

    vec2i mousePos = canvas->getMousePosition() + canvasPos;

    EllipseBounds bounds = getEllipseBounds(beginEllipsePos_, mousePos);

    vec2i topLeft     = bounds.topLeft;
    vec2i bottomRight = bounds.bottomRight;
    
    const unsigned ellipseYSize = bottomRight.y - topLeft.y;
    const unsigned ellipseXSize = bottomRight.x - topLeft.x;

    std::unique_ptr<IEllipseShape> ellipse = IEllipseShape::create(ellipseXSize, ellipseYSize);
    if (!ellipse)
        return updateStateRes;

    static const Color redColor{0xFF, 0x00, 0x00, 0xFF};
    ellipse->setFillColor(redColor);
    ellipse->setOutlineThickness(0);
    ellipse->setPosition(topLeft);

    const IImage* image = ellipse->getImage();
    if (!image)
        return updateStateRes;

    copyImageToLayer(tempLayer, image, canvasPos, image->getSize());

    return true;
}

} // namespace ps

bool loadPlugin() // onLoadPlugin
{
    auto buttonSprite  = std::unique_ptr<ISprite>(ISprite::create());
    auto buttonTexture = std::unique_ptr<ITexture>(ITexture::create());

    buttonTexture.get()->loadFromFile("media/textures/paintbrush.png");

    buttonSprite->setTexture(buttonTexture.get());

    IWindowContainer* rootWindow = getRootWindow();
    assert(rootWindow);
    auto toolBar = static_cast<IBar*>(rootWindow->getWindowById(kToolBarWindowId));
    assert(toolBar);

    auto info = toolBar->getNextChildInfo();
    auto pos = info.pos;
    vec2u size = { static_cast<unsigned int>(info.size.x),  
                   static_cast<unsigned int>(info.size.y) };

    buttonSprite->setPosition(pos.x, pos.y);
    
    auto spriteSize = buttonSprite->getSize();
    buttonSprite->setScale(static_cast<double>(size.x) / spriteSize.x, static_cast<double>(size.y) / spriteSize.y);
    std::unique_ptr<ps::ABarButton> button{ new ps::EllipseButton(std::move(buttonSprite), std::move(buttonTexture)) };

    button->setPos(pos);
    button->setSize(size);

    assert(rootWindow->getWindowById(kToolBarWindowId));
    
    static_cast<IBar*>(rootWindow->getWindowById(kToolBarWindowId))->addWindow(std::move(button));

    return true;
}

void unloadPlugin()
{
    return;
}
