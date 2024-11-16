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

#include <iostream>

using namespace ps;
using namespace psapi;
using namespace psapi::sfm;

namespace
{

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

class EllipseButton : public ASpritedBarButton 
{
public:
    EllipseButton() = default;
    EllipseButton(std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture);

    bool update(const IRenderWindow* renderWindow, const Event& event) override;
    void draw(IRenderWindow *window) override;

    void setParent(const IWindow* parent) override;

private:
    const IBar* parent_;
    
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

std::unique_ptr<IEllipseShape> createEllipse(const vec2i& beginEllipsePos, const ICanvas* canvas)
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

    static const Color redColor{0xFF, 0x00, 0x00, 0xFF};
    ellipse->setFillColor(redColor);
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

    ellipse_ = createEllipse(beginEllipsePos_, canvas);

    if (ellipse_)
        const_cast<IRenderWindow*>(renderWindow)->draw(ellipse_.get()); // crutch

    return true;
}

void EllipseButton::draw(IRenderWindow *window)
{
    ASpritedBarButton::draw(window, parent_);

    if (ellipse_)
        window->draw(ellipse_.get());
}

void EllipseButton::setParent(const IWindow* parent)
{
    parent_ = dynamic_cast<const IBar*>(parent);
    assert(parent_);
}


} // namespace anonymous

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

    std::unique_ptr<ps::ASpritedBarButton> button{ new EllipseButton(std::move(buttonSprite), std::move(buttonTexture)) };

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
