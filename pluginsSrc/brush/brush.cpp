#include "brush.hpp"
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

class BrushButton : public ABarButton 
{
public:
    BrushButton() = default;
    BrushButton(std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture);

    virtual bool update(const IRenderWindow* renderWindow, const Event& event) override;

protected:
    bool released_;
};

BrushButton::BrushButton(std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture)
{
    mainSprite_ = std::move(sprite);
    mainTexture_ = std::move(texture);
}

bool BrushButton::update(const IRenderWindow* renderWindow, const Event& event)
{
    bool hovered = isHovered(Mouse::getPosition(renderWindow));
    bool pressed = isPressed(event);

    bool clicked = isClicked(event);

    if (clicked)
    {
        if (!released_)
            state_ = State::Released;
        else
            state_ = State::Normal;

        released_ = !released_;
    }
    
    if (!released_)
    {
        if (hovered)      state_ = State::Hover;
        else if (pressed) state_ = State::Press;
        else              state_ = State::Normal;

        return true;
    }

    ICanvas* canvas = static_cast<ICanvas*>(getRootWindow()->getWindowById(kCanvasWindowId));

    if (!canvas)
    {
        std::cerr << "CANVAS NOT FOUND!\n";
        assert(0);
    }

    if (!canvas->isPressed())
        return true;

    size_t activeLayerIndex = canvas->getActiveLayerIndex();

    auto mousePos = canvas->getMousePosition();

    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            canvas->getLayer(activeLayerIndex)->setPixel({mousePos.x + i, mousePos.y + j}, {0xFF, 0x00, 0x00, 0xFF});
    
    return true;
}

} // namespace ps

bool loadPlugin()
{
    std::cerr << "BRUSH PLUGIN\n";
    auto buttonSprite = std::unique_ptr<ISprite>(ISprite::create());
    auto buttonTexture = std::unique_ptr<ITexture>(ITexture::create());

    buttonTexture.get()->loadFromFile("media/textures/paintbrush.png");

    std::cerr << "loaded from file\n";
    buttonSprite->setTexture(buttonTexture.get());

    IWindowContainer* rootWindow = getRootWindow();
    assert(rootWindow);
    auto toolBar = static_cast<IBar*>(rootWindow->getWindowById(kToolBarWindowId));
    assert(toolBar);
    std::cerr << "got toolbar\n";

    auto info = toolBar->getNextChildInfo();
    auto pos = info.pos;
    vec2u size = { static_cast<unsigned int>(info.size.x),  
                   static_cast<unsigned int>(info.size.y) };

    buttonSprite->setPosition(pos.x, pos.y);
    
    auto spriteSize = buttonSprite->getSize();
    buttonSprite->setScale(static_cast<double>(size.x) / spriteSize.x, static_cast<double>(size.y) / spriteSize.y);
    std::unique_ptr<ps::ABarButton> button{ new ps::BrushButton(std::move(buttonSprite), std::move(buttonTexture)) };
    std::cerr << "SET SPRITE\n";

    button->setPos(pos);
    button->setSize(size);

    std::cerr << "GETTING TOOLBAR\n";
    assert(rootWindow->getWindowById(kToolBarWindowId));
    
    std::cerr << "BUTTON ADR - " << button.get() << std::endl;
    
    static_cast<IBar*>(rootWindow->getWindowById(kToolBarWindowId))->addWindow(std::move(button));

    std::cerr << "RETURNING\n";
    return true;
}

void unloadPlugin()
{
    return;
}
