#include "windows.hpp"

#include <iostream>
#include <cassert>

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
    //std::cerr << "CHECK IS HOVERED, POS: " << pos_.x << " " << pos_.y << " SIZE: " << size_.x << " " << size_.y << "MOUSE POS: " << mousePos.x << " " << mousePos.y << std::endl;
    return mousePos.x >= pos_.x && mousePos.x < static_cast<int>(pos_.x + size_.x)
        && mousePos.y >= pos_.y && mousePos.y < static_cast<int>(pos_.y + size_.y);
}

bool AWindow::checkIsClicked(const Event& event, vec2i mousePos)
{
    return checkIsHovered(mousePos) && event.type == Event::MouseButtonReleased;
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

void AWindow::setParent(const IWindow* parent)
{
    parent_ = parent;
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


// AWindowContainer implementation

AWindowContainer::AWindowContainer(vec2i pos, vec2u size, wid_t id) : AWindow(pos, size, id)
{
}

bool AWindowContainer::isWindowContainer() const        { return true; }

wid_t AWindowContainer::getId() const                   { return AWindow::getId(); }
vec2i AWindowContainer::getPos() const                  { return AWindow::getPos(); }
vec2u AWindowContainer::getSize() const                 { return AWindow::getSize(); }

void AWindowContainer::forceActivate()                  { AWindow::forceActivate(); }
void AWindowContainer::forceDeactivate()                { AWindow::forceDeactivate(); }
bool AWindowContainer::isActive() const                 { return AWindow::isActive(); }
void AWindowContainer::setParent(const IWindow* parent) { AWindow::setParent(parent); }

// other functions implementation

vec2i shrinkPosToBoundary(const vec2i& objectPos, const vec2u& objectSize, 
                          const vec2i& boundaryPos, const vec2u& boundarySize)
{
    assert(objectSize.x <= boundarySize.x);
    assert(objectSize.y <= boundarySize.y);

    vec2i result = objectPos;
    result.x = std::max(result.x, boundaryPos.x);
    result.x = std::min(result.x, static_cast<int>(boundaryPos.x + boundarySize.x - objectSize.x));
    result.y = std::max(result.y, boundaryPos.y);
    result.y = std::min(result.y, static_cast<int>(boundaryPos.y + boundarySize.y - objectSize.y));

    return result;
}

bool checkIsHovered(vec2i mousePos, const vec2i& pos, const vec2u& size)
{
    return mousePos.x >= pos.x && mousePos.x < static_cast<int>(pos.x + size.x)
        && mousePos.y >= pos.y && mousePos.y < static_cast<int>(pos.y + size.y);
}

bool updateIsPressed(const Event& event, bool prevPressedState, bool isHovered)
{
    bool pressedRightNow  = isHovered && event.type == Event::MouseButtonPressed;
    bool releasedRightNow = isHovered && event.type == Event::MouseButtonReleased;

    bool isPressed = prevPressedState;
    if (pressedRightNow)  isPressed = true;
    if (releasedRightNow) isPressed = false;
    if (isPressed && !isHovered && event.type == Event::MouseButtonReleased) isPressed = false;

    return isPressed;
}

bool checkIsClicked(const Event& event, bool isHovered)
{
    return isHovered && event.type == Event::MouseButtonReleased;
}

} // namespace ps