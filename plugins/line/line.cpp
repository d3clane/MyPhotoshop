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

namespace
{

class LineButton : public ABarButton 
{
public:
    LineButton() = default;
    LineButton(std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture);

    bool update(const IRenderWindow* renderWindow, const Event& event) override;
    void draw(IRenderWindow* renderWindow) override;

private:
    bool canvasIsAlreadyPressed_ = false;

    vec2i lineBeginPos_;

    std::unique_ptr<IRectangleShape> line_; // crutch to not to copy every time on canvas
};

void copyLineToLayer(ILayer* layer, const IRectangleShape* line, vec2i canvasPos)
{
    const IImage* image = line->getImage();
    if (!image)
        return;

    copyImageToLayer(layer, image, canvasPos, image->getSize());
}


} // namespace anonymous

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
    vec2i canvasPos = canvas->getPos();

    if (!canvas->isPressed() && canvasIsAlreadyPressed_)
    {
        copyLineToLayer(activeLayer, line_.get(), canvasPos);
        line_.reset();
    }
    
    if (!canvas->isPressed())
    {
        canvasIsAlreadyPressed_ = false;
        return updateStateRes;
    }

    if (!canvasIsAlreadyPressed_)
    {
        lineBeginPos_ = canvas->getMousePosition() + canvasPos;
        canvasIsAlreadyPressed_ = true;
    }

    vec2i mousePos = canvas->getMousePosition() + canvasPos;

    const float lineLength = len(mousePos, lineBeginPos_);
    const float angle = std::atan2(mousePos.y - lineBeginPos_.y, mousePos.x - lineBeginPos_.x);

    static const size_t thickness = 10;
    line_ = IRectangleShape::create(lineLength, thickness);

    static const Color redColor{0xFF, 0x00, 0x00, 0xFF};
    line_->setFillColor(redColor);
    line_->setOutlineThickness(0);
    line_->setRotation(angle * 180 / M_PI);
    line_->setPosition(lineBeginPos_);

    const_cast<IRenderWindow*>(renderWindow)->draw(line_.get()); // crutch
    
    return true;
}

void LineButton::draw(IRenderWindow* renderWindow)
{
    ABarButton::draw(renderWindow);

    if (line_)
        renderWindow->draw(line_.get());
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
