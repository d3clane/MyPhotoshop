#include "bars/ps_bar.hpp"


#include <cassert>

namespace ps
{

void ABarButton::draw(IRenderWindow* renderWindow) 
{
    renderWindow->draw(mainSprite_.get());
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
    parent_ = parent;
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

// ABar implementation

void ABar::drawChildren(IRenderWindow* renderWindow) 
{
    for (const auto& window : windows_)
        window->draw(renderWindow);
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