#include "spray.hpp"

#include <string>
#include <cassert>

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

class SprayButton : public ASpritedBarButton 
{
public:
    SprayButton() = default;
    SprayButton(std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture);

    bool update(const IRenderWindow* renderWindow, const Event& event) override;

    void draw(IRenderWindow* renderWindow) override;
    void setParent(const IWindow* parent) override;

private:
    const IBar* parent_;
};

SprayButton::SprayButton(std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture)
{
    mainSprite_ = std::move(sprite);
    mainTexture_ = std::move(texture);
}

bool SprayButton::update(const IRenderWindow* renderWindow, const Event& event)
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

    if (event.type != Event::MouseButtonReleased)
        return true;

    size_t activeLayerIndex = canvas->getActiveLayerIndex();
    ILayer* activeLayer = canvas->getLayer(activeLayerIndex);

    auto mousePos = canvas->getMousePosition();
    for (int i = -5; i < 5; ++i)
        for (int j = -5; j < 5; ++j)
            activeLayer->setPixel({mousePos.x + i, mousePos.y + j}, {0xFF, 0x00, 0x00, 0xFF});
    
    return updatedState;
}

void SprayButton::draw(IRenderWindow* renderWindow) { ASpritedBarButton::draw(renderWindow, parent_); }

void SprayButton::setParent(const IWindow* parent)
{
    parent_ = dynamic_cast<const IBar*>(parent);
    assert(parent_);
}

} // namespace ps

bool loadPlugin()
{
    auto buttonSprite  = std::unique_ptr<ISprite>(ISprite::create());
    auto buttonTexture = std::unique_ptr<ITexture>(ITexture::create());

    buttonTexture.get()->loadFromFile("media/textures/spray.png");

    buttonSprite->setTexture(buttonTexture.get());

    IWindowContainer* rootWindow = getRootWindow();
    assert(rootWindow);
    auto toolBar = static_cast<IBar*>(rootWindow->getWindowById(kToolBarWindowId));
    assert(toolBar);

    auto info = toolBar->getNextChildInfo();
    auto pos = info.pos;
    vec2u size = { static_cast<unsigned>(info.size.x), static_cast<unsigned>(info.size.y) };

    buttonSprite->setPosition(pos.x, pos.y);
    
    auto spriteSize = buttonSprite->getSize();
    buttonSprite->setScale(static_cast<float>(size.x) / static_cast<float>(spriteSize.x), 
                           static_cast<float>(size.y) / static_cast<float>(spriteSize.y));
    std::unique_ptr<ps::ASpritedBarButton> button{ new ps::SprayButton(std::move(buttonSprite), std::move(buttonTexture)) };
    
    button->setPos(pos);
    button->setSize(size);

    static_cast<IBar*>(rootWindow->getWindowById(kToolBarWindowId))->
        addWindow(std::unique_ptr<IBarButton>(button.release()));

    return true;
}

void unloadPlugin()
{
    return;
}
