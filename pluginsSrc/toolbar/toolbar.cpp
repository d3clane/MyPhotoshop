#include "toolbar.hpp"

#include <cassert>
#include <iostream>
#include <memory>

using namespace ps;

extern "C" 
{
    
bool loadPlugin() 
{
    vec2i pos  = {0,   0  };
    vec2u size = {100, 600};
    auto toolbar = std::make_unique<Toolbar>(pos, size);
    //std::cerr << "TB ID - " << toolbar.get()->getId() << "\n";

    getRootWindow()->addWindow(std::move(toolbar));
    
    return true;
}

void unloadPlugin() 
{
    getRootWindow()->removeWindow(kToolBarWindowId);
}

}

Toolbar::Toolbar(vec2i pos, vec2u size) 
{
    pos_ = pos;
    size_ = size;

    texture_ = std::move(ITexture::create());
    texture_->loadFromFile("media/textures/toolbar.png");

    sprite_ = std::move(ISprite::create());
    sprite_->setTexture(texture_.get());

    vec2u spriteSize = sprite_->getSize();

    sprite_->setScale(static_cast<double>(size_.x) / spriteSize.x, static_cast<double>(size_.y) / spriteSize.y);
    sprite_->setPosition(pos_.x, pos_.y);
}

ChildInfo Toolbar::getNextChildInfo() const 
{
    vec2i pos = {gapSize_ + (gapSize_ + childSize_.x) * nextChildIndex_, gapSize_};

    return {pos, childSize_};
}

void Toolbar::finishButtonDraw(IRenderWindow* renderWindow, const IBarButton* button) const
{
    // TODO: load sprites
#if 0
    switch (button->state()) 
    {
        case IBarButton::State::Normal:
            break;
        case IBarButton::State::Hover:
            renderWindow->draw(hoverSprite);
            break;
        case IBarButton::State::Press:
            renderWindow->draw(pressSprite);
            break;
        case IBarButton::State::Released:
            renderWindow->draw();
            break;
    }
#endif
}

void Toolbar::addWindow(std::unique_ptr<IWindow> window)
{
    ABarButton* button = nullptr;
    try 
    {
        button = dynamic_cast<ABarButton*>(window.get());
    }
    catch(...)
    {
        std::cerr << "Failed to cast window to button\n";
        assert(false);
        std::terminate();
    }

    if (!button)
    {
        std::cerr << "Failed to cast window to button\n";
        assert(false);
        std::terminate();
    }

    windows_.push_back(std::unique_ptr<ABarButton>(button));
}

void Toolbar::removeWindow(wid_t id)
{
    for (auto it = windows_.begin(); it != windows_.end(); it++) 
    {
        if ((*it)->getId() == id) 
        {
            windows_.erase(it);
            return;
        }
    }
}