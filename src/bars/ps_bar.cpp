#include "bars/ps_bar.hpp"

#include <cassert>
#include <iostream>

namespace ps
{

void ABarButton::draw(IRenderWindow* renderWindow) 
{
    renderWindow->draw(mainSprite_.get());

    assert(parent_);
    parent_->finishButtonDraw(renderWindow, this);
}

IWindow* ABarButton::getWindowById(wid_t id) 
{
    return const_cast<IWindow*>(const_cast<const ABarButton*>(this)->getWindowById(id));
}

const IWindow* ABarButton::getWindowById(wid_t id) const {
    if (id == id_)
        return this;

    return nullptr;
}

vec2i ABarButton::getPos() const 
{
    return pos_;
}

vec2u ABarButton::getSize() const 
{
    return size_;
}

wid_t ABarButton::getId() const 
{
    return id_;
}

void ABarButton::setParent(const IWindow* parent) 
{
    try
    {
        const ABar* parentBar = static_cast<const ABar*>(parent);

        std::cerr << "PARENT BAR ADR - " << parentBar << std::endl;
        assert(parentBar);
        parent_ = parentBar;
    }
    catch(...)
    {
        std::cerr << "Can't cast parent to ABar\n";
        assert(false);
        std::terminate();
    }
}

void ABarButton::forceDeactivate() 
{
    isActive_ = false;
}

void ABarButton::forceActivate() 
{
    isActive_ = true;
}

bool ABarButton::isActive() const 
{
    return isActive_;
}

bool ABarButton::isWindowContainer() const
{
    return false;
}

void ABarButton::setState(State state) 
{
    state_ = state;
}

ABarButton::State ABarButton::getState() const 
{
    return state_;
}

bool ABarButton::isHovered(vec2i mousePos)
{
    return mousePos.x >= pos_.x && mousePos.x < pos_.x + size_.x &&
           mousePos.y >= pos_.y && mousePos.y < pos_.y + size_.y;
}

bool ABarButton::isPressed (const Event& event)
{
    return (event.type == event.MouseButtonPressed && isHovered({event.mouseButton.x, event.mouseButton.y}));
}

bool ABarButton::isClicked(const Event& event)
{
    return (event.type == event.MouseButtonReleased && isHovered({event.mouseButton.x, event.mouseButton.y}));
}

void ABarButton::setPos (vec2i pos)
{
    pos_ = pos;

    mainSprite_->setPosition(pos.x, pos.y);
}

void ABarButton::setSize(vec2u size)
{
    size_ = size;

    mainSprite_->setScale(1.f, 1.f);
    auto spriteSize = mainSprite_->getSize();

    mainSprite_->setScale(static_cast<double>(size.x) / spriteSize.x, 
                          static_cast<double>(size.y) / spriteSize.y);
}

// ABar implementation

void ABar::drawChildren(IRenderWindow* renderWindow) 
{
    for (const auto& window : windows_)
    {
        assert(window.get());
        window->draw(renderWindow);
    }
}

bool ABar::updateChildren(const IRenderWindow* renderWindow, const sfm::Event& event) 
{
    bool updatedSomeone = false;
    for (const auto& window : windows_) 
        updatedSomeone |= window->update(renderWindow, event);

    return updatedSomeone;
}

const IWindow* ABar::getWindowById(wid_t id) const 
{
    if (id_ == id)
        return this;
        
    for (auto& window : windows_) 
    {
        const IWindow* result = window->getWindowById(id);
        if (result)
            return result;
    }

    return nullptr;
}

ABar::~ABar() = default;

void ABar::draw(IRenderWindow* renderWindow)
{
    renderWindow->draw(sprite_.get());
    
    drawChildren(renderWindow);
}

bool ABar::update(const IRenderWindow* renderWindow, const sfm::Event& event) 
{
    bool updatedSomeone = updateChildren(renderWindow, event);

    return updatedSomeone;
}

IWindow* ABar::getWindowById(wid_t id) 
{
    return const_cast<IWindow*>(static_cast<const ABar*>(this)->getWindowById(id));
}

vec2i ABar::getPos()  const 
{
    return pos_;
}

vec2u ABar::getSize() const 
{
    return size_;
}

wid_t ABar::getId() const 
{
    return kToolBarWindowId;
}

void ABar::setParent(const IWindow* parent) 
{
    parent_ = parent;
}

void ABar::forceDeactivate() 
{
    isActive_ = false;
}

void ABar::forceActivate() 
{
    isActive_ = true;
}

bool ABar::isActive() const 
{
    return isActive_;
}

bool ABar::isWindowContainer() const
{
    return true;
}

} // namespace ps