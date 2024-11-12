#include "line.hpp"
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

namespace 
{

using namespace ps;
using namespace psapi;
using namespace psapi::sfm;

class LineButton : public ASpritedBarButton 
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

std::unique_ptr<IRectangleShape> createLine(vec2i beginPos, const ICanvas* canvas)
{
    vec2i mousePos = canvas->getMousePosition() + canvas->getPos();
    mousePos = shrinkPosToBoundary(mousePos, vec2u{0, 0}, canvas->getPos(), canvas->getSize());

    const float lineLength = len(beginPos, mousePos);
    const float angle = std::atan2(mousePos.y - beginPos.y, mousePos.x - beginPos.x);

    static const size_t thickness = 10;
    std::unique_ptr<IRectangleShape> line = IRectangleShape::create(lineLength, thickness);

    static const Color redColor{0xFF, 0x00, 0x00, 0xFF};
    line->setFillColor(redColor);
    line->setOutlineThickness(0);
    line->setRotation(angle * 180 / M_PI);
    line->setPosition(beginPos);

    return line;
}

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

    line_ = createLine(lineBeginPos_, canvas);

    if (line_)
        const_cast<IRenderWindow*>(renderWindow)->draw(line_.get()); // crutch
    
    return true;
}

void LineButton::draw(IRenderWindow* renderWindow)
{
    ASpritedBarButton::draw(renderWindow);

    if (line_)
        renderWindow->draw(line_.get());
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
    buttonSprite->setScale(static_cast<double>(size.x) / spriteSize.x, static_cast<double>(size.y) / spriteSize.y);
    std::unique_ptr<ps::ASpritedBarButton> button{ new LineButton(std::move(buttonSprite), std::move(buttonTexture)) };

    button->setPos(pos);
    button->setSize(size);

    assert(rootWindow->getWindowById(kToolBarWindowId));
    
    static_cast<IBar*>(rootWindow->getWindowById(kToolBarWindowId))->
        addWindow(std::unique_ptr<IBarButton>(button.release()));

    return true;
}

void unloadPlugin()
{
    return;
}
