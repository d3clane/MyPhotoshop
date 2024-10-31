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
    vec2u size = {100, 1080};
    auto toolbar = std::make_unique<Toolbar>(pos, size);

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

    loadSprite(SpriteType::Hover,   "media/textures/toolbar_button_hover.png");
    loadSprite(SpriteType::Press,   "media/textures/toolbar_button_press.png");
    loadSprite(SpriteType::Release, "media/textures/toolbar_button_release.png");
}

void Toolbar::loadSprite(SpriteType type, const std::string& path)
{
    auto& spriteInfo = sprites_[static_cast<size_t>(type)];
    spriteInfo.texture = std::move(ITexture::create());
    spriteInfo.texture->loadFromFile(path);

    spriteInfo.sprite = std::move(ISprite::create());
    spriteInfo.sprite->setTexture(spriteInfo.texture.get());

    // TODO: дикий костыль 1.2
    spriteInfo.sprite->setScale(1.2 * static_cast<double>(childSize_.x) / spriteInfo.sprite->getSize().x, 
                                1.2 * static_cast<double>(childSize_.y) / spriteInfo.sprite->getSize().y);

    auto color = spriteInfo.sprite->getColor();
    static const uint8_t goodAlpha = 100;
    spriteInfo.sprite->setColor(Color(color.r, color.g, color.b, goodAlpha));
}

ChildInfo Toolbar::getNextChildInfo() const 
{
    vec2i pos = {gapSize_, gapSize_ + (gapSize_ + childSize_.x) * nextChildIndex_};

    nextChildIndex_++;

    return {pos, childSize_};
}

void Toolbar::finishButtonDraw(IRenderWindow* renderWindow, const IBarButton* button) const
{
    //TODO: дикий костыль
    sprites_[static_cast<size_t>(SpriteType::Hover)]  .sprite->setPosition(button->getPos().x - 6, button->getPos().y - 6);
    sprites_[static_cast<size_t>(SpriteType::Release)].sprite->setPosition(button->getPos().x - 6, button->getPos().y - 6);
    sprites_[static_cast<size_t>(SpriteType::Press)]  .sprite->setPosition(button->getPos().x - 6, button->getPos().y - 6);

    switch (button->getState()) 
    {
        case IBarButton::State::Normal:
            break;
        case IBarButton::State::Hover:
            renderWindow->draw(sprites_[static_cast<size_t>(SpriteType::Hover)].sprite.get());
            break;
        case IBarButton::State::Press:
            renderWindow->draw(sprites_[static_cast<size_t>(SpriteType::Press)].sprite.get());
            break;
        case IBarButton::State::Released:
            renderWindow->draw(sprites_[static_cast<size_t>(SpriteType::Release)].sprite.get());
            break;

        default:
            assert(false);
            std::terminate();
            break;
    }
}

void Toolbar::addWindow(std::unique_ptr<IWindow> window)
{
    ABarButton* button = nullptr;
    try
    {
        button = dynamic_cast<ABarButton*>(window.get());
        assert(button);
        button->setParent(this);
    }
    catch(...)
    {
        std::cerr <<"Failed to cast window to button\n";
        assert(false);
        std::terminate();
    }

    windows_.push_back(std::unique_ptr<ABarButton>(button));
    window.release();
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