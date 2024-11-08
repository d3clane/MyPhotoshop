#include "line.hpp"
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

class LineButton : public ABarButton 
{
public:
    LineButton() = default;
    LineButton(std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture);

    virtual bool update(const IRenderWindow* renderWindow, const Event& event) override;

private:
    bool canvasIsAlreadyPressed = false;

    vec2i lineBeginPos_;
};

LineButton::LineButton(std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture)
{
    mainSprite_ = std::move(sprite);
    mainTexture_ = std::move(texture);
}

bool LineButton::update(const IRenderWindow* renderWindow, const Event& event)
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

    if (!canvas->isPressed() && canvasIsAlreadyPressed)
        copyLayerToLayer(activeLayer, tempLayer, canvas->getSize());
    
    canvas->cleanTempLayer();

    if (!canvas->isPressed())
    {
        canvasIsAlreadyPressed = false;
        return updateStateRes;
    }

    vec2i canvasPos = canvas->getPos();

    if (!canvasIsAlreadyPressed)
    {
        lineBeginPos_ = canvas->getMousePosition() + canvasPos;
        canvasIsAlreadyPressed = true;
    }

    vec2i mousePos = canvas->getMousePosition() + canvasPos;

    const float lineLength = len(mousePos, lineBeginPos_);
    const float angle = std::atan2(mousePos.y - lineBeginPos_.y, mousePos.x - lineBeginPos_.x);

    static const size_t thickness = 10;
    std::unique_ptr<IRectangleShape> line = IRectangleShape::create(lineLength, thickness);

    static const Color redColor{0xFF, 0x00, 0x00, 0xFF};
    line->setFillColor(redColor);

    line->setOutlineThickness(0);
    line->setRotation(angle * 180 / M_PI);
    line->setPosition(lineBeginPos_);

    const IImage* image = line->getImage();
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
    std::unique_ptr<ps::ABarButton> button{ new ps::LineButton(std::move(buttonSprite), std::move(buttonTexture)) };

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
