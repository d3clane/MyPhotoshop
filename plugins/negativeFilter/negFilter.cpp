#include "negFilter.hpp"
#include "bars/ps_bar.hpp"

#include "negFilter.hpp"

#include <string>
#include <cassert>

#include "api/api_sfm.hpp"
#include "api/api_photoshop.hpp"
#include "api/api_bar.hpp"
#include "api/api_canvas.hpp"
#include "canvas/canvas.hpp"

#include "bars/ps_bar.hpp"
#include "windows/windows.hpp"
#include "filters/filters.hpp"

#include <iostream>

using namespace ps;
using namespace psapi;
using namespace psapi::sfm;

namespace
{

class NegativeFilterButton : public ASpritedBarButton 
{
public:
    NegativeFilterButton() = default;
    NegativeFilterButton(std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture);

    bool update(const IRenderWindow* renderWindow, const Event& event) override;
    void draw(IRenderWindow *window) override;

    void setParent(const IWindow* parent) override;

private:
    const IBar* parent_;
};

NegativeFilterButton::NegativeFilterButton(std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture)
{
    mainSprite_ = std::move(sprite);
    mainTexture_ = std::move(texture);
}

bool NegativeFilterButton::update(const IRenderWindow* renderWindow, const Event& event)
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

    vec2u canvasSize = canvas->getSize();

    std::vector<std::vector<Color>> pixels = getLayerScreenIn2D(activeLayer, canvasSize);
    std::vector<std::vector<Color>> negative = getNegative(pixels);
    
    std::vector<std::vector<Color>> basRelief = getBasRelief(pixels, negative);
    copyPixelsToLayer(activeLayer, negative);
    
    state_ = State::Normal;

    return true;
}

void NegativeFilterButton::draw(IRenderWindow *window) 
{
    ASpritedBarButton::draw(window, parent_);
}

void NegativeFilterButton::setParent(const IWindow* parent)
{
    parent_ = dynamic_cast<const IBar*>(parent);
    assert(parent_);
}

}

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
    buttonSprite->setScale(static_cast<float>(size.x) / static_cast<float>(spriteSize.x), static_cast<float>(size.y) / static_cast<float>(spriteSize.y));
    std::unique_ptr<ps::ASpritedBarButton> button{ new NegativeFilterButton(std::move(buttonSprite), std::move(buttonTexture)) };

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
