#include "windows.hpp"

namespace ps
{

// AWindow implementation

AWindow::AWindow(vec2i pos, vec2u size, wid_t id) : id_(id), pos_(pos), size_(size)
{
}

wid_t AWindow::getId() const
{
    return id_;
}

IWindow* AWindow::getWindowById(wid_t id)
{
    return id == id_ ? this : nullptr;
}

const IWindow* AWindow::getWindowById(wid_t id) const
{
    return id == id_ ? this : nullptr;
}

vec2i AWindow::getPos() const
{
    return pos_;
}

vec2u AWindow::getSize() const
{
    return size_;
}

void AWindow::setParent(const IWindow* parent)
{
    parent_ = parent;
}

void AWindow::forceActivate()
{
    isActive_ = true;
}

void AWindow::forceDeactivate()
{
    isActive_ = false;
}

bool AWindow::isActive() const
{
    return isActive_;
}

bool AWindow::isWindowContainer() const
{
    return false;
}

bool AWindow::checkIsHovered(vec2i mousePos)
{
    return mousePos.x >= pos_.x && mousePos.x < static_cast<int>(pos_.x + size_.x)
        && mousePos.y >= pos_.y && mousePos.y < static_cast<int>(pos_.y + size_.y);
}

bool AWindow::updateIsPressed(const Event& event, bool prevPressedState, vec2i mousePos)
{
    bool hoveredRightNow  = checkIsHovered(mousePos);
    bool pressedRightNow  = hoveredRightNow && event.type == Event::MouseButtonPressed;
    bool releasedRightNow = hoveredRightNow && event.type == Event::MouseButtonReleased;

    bool isPressed = prevPressedState;
    if (pressedRightNow)  isPressed = true;
    if (releasedRightNow) isPressed = false;
    if (isPressed && !hoveredRightNow && event.type == Event::MouseButtonReleased) isPressed = false;

    return isPressed;
}

// ASpritedWindow implementation

ASpritedWindow::ASpritedWindow(
    vec2i pos, vec2u size, wid_t id, 
    std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture
) : AWindow(pos, size, id), sprite_(std::move(sprite)), texture_(std::move(texture))
{
}

void ASpritedWindow::draw(IRenderWindow* renderWindow)
{
    sprite_->draw(renderWindow);
}

// AWindowVector implementation

AWindowVector::AWindowVector(vec2i pos, vec2u size, wid_t id) : AWindow(pos, size, id)
{
}

bool AWindowVector::updateChildren(const IRenderWindow* renderWindow, const sfm::Event& event)
{
    bool updatedSomeone = false;

    for (auto& window : windows_)
        updatedSomeone |= window->update(renderWindow, event);
    
    return updatedSomeone;
}

void AWindowVector::drawChildren(IRenderWindow* renderWindow)
{
    for (const auto& window : windows_)
        window->draw(renderWindow);
}

// ASpritedWindowVector implementation

ASpritedWindowVector::ASpritedWindowVector(
    vec2i pos, vec2u size, wid_t id, 
    std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture
) : AWindowVector(pos, size, id), sprite_(std::move(sprite)), texture_(std::move(texture))
{
}

void ASpritedWindowVector::draw(IRenderWindow* renderWindow)
{
    sprite_->draw(renderWindow);

    drawChildren(renderWindow);
}


} // namespace ps