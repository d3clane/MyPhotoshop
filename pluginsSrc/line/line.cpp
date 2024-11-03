#include "line.hpp"
#include <string>
#include <dlfcn.h>

#include "api/api_sfm.hpp"
#include "api/api_photoshop.hpp"
#include "api/api_bar.hpp"
#include "api/api_canvas.hpp"

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

    vec2i mouseBeginPos_;
};

LineButton::LineButton(std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture)
{
    mainSprite_ = std::move(sprite);
    mainTexture_ = std::move(texture);
}

bool LineButton::update(const IRenderWindow* renderWindow, const Event& event)
{
    bool updateStateRes = updateState(renderWindow, event);

    if (!released_)
        return updateStateRes;

    ICanvas* canvas = static_cast<ICanvas*>(getRootWindow()->getWindowById(kCanvasWindowId));
    if (!canvas)
    {
        std::cerr << "CANVAS NOT FOUND!\n";
        assert(0);
    }

    size_t activeLayerIndex = canvas->getActiveLayerIndex();
    ILayer* activeLayer = canvas->getLayer(activeLayerIndex);

    ILayer* tempLayer = canvas->getTempLayer();
    int cnt = 0;
    if (!canvas->isPressed() && canvasIsAlreadyPressed)
    {
        for (size_t x = 0; x < canvas->getSize().x; ++x)
        {
            for (size_t y = 0; y < canvas->getSize().y; ++y)
            {
                Color tempLayerPixelColor = tempLayer->getPixel({static_cast<int>(x), static_cast<int>(y)});
                if (tempLayerPixelColor.a == 0)
                    continue;
                activeLayer->setPixel({static_cast<int>(x), static_cast<int>(y)}, tempLayerPixelColor);
            }
        }
    }

    canvas->cleanTempLayer();

    if (!canvas->isPressed())
    {
        canvasIsAlreadyPressed = false;
        return updateStateRes;
    }

    vec2i canvasPos = canvas->getPos();

    if (!canvasIsAlreadyPressed)
    {
        mouseBeginPos_ = canvas->getMousePosition() + canvasPos;
        canvasIsAlreadyPressed = true;
    }

    auto mousePos  = canvas->getMousePosition() + canvasPos;

    static const Color redColor{0xFF, 0x00, 0x00, 0xFF};
    static const size_t thickness = 10;

    const float lineLength = len(mousePos, mouseBeginPos_);
    const float angle = std::atan2(mousePos.y - mouseBeginPos_.y, mousePos.x - mouseBeginPos_.x);

    std::unique_ptr<IRectangleShape> line = IRectangleShape::create(lineLength, thickness);
    line->setFillColor(redColor);
    line->setOutlineThickness(0);
    line->setRotation(angle * 180 / M_PI);
    line->setPosition(mouseBeginPos_);

    const IImage* image = line->getImage();
    if (!image)
        return updateStateRes;

    vec2u imageSize = image->getSize();
    assert(imageSize.x == 1920 && imageSize.y == 1080);

    for (size_t x = 0; x < imageSize.x; ++x)
    {
        for (size_t y = 0; y < imageSize.y; ++y)
        {
            tempLayer->setPixel({x - canvasPos.x, y - canvasPos.y}, image->getPixel(x, y));
        }
    }

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
