#include "scrollbar.hpp"

#include <cassert>
#include <iostream>

namespace ps
{

// PressButton implementation

PressButton::PressButton(vec2i pos, vec2u size, wid_t id) : AWindow(pos, size, id)
{
}

PressButton::State PressButton::getState() const
{
    return state_;
}

void PressButton::setShape(std::unique_ptr<IRectangleShape> shape, State state)
{
    auto& myShape = shapes_[static_cast<size_t>(state)];

    myShape = std::move(shape);
    myShape->setSize(size_);
    myShape->setPosition(pos_);
}

// Scrollbar implementation

ScrollBar::ScrollBar(vec2i pos, vec2u size, wid_t id) 
    : AWindowContainer(pos, size, id), moveButton_(std::make_unique<ScrollBarButton>(pos, size, id))
{
}

bool ScrollBar::update(const IRenderWindow* renderWindow, const Event& event)
{
    if (!isActive_)
        return false;
    
    bool eventUsed = moveButton_->update(renderWindow, event);
    if (eventUsed)
        return true;

    vec2i mousePos = sfm::Mouse::getPosition();
    bool isHovered = checkIsHovered(mousePos);
    bool isPressed = (event.type == Event::MouseButtonPressed);

    if (!isHovered || !isPressed)
        return false;

    vec2i newPos{mousePos.x - pos_.x, mousePos.y - pos_.y};
    newPos = shrinkPosToBoundaries(newPos, size_);
    moveButton_->setPos(newPos);

    return true;
}

void ScrollBar::draw(IRenderWindow* renderWindow)
{
    if (shape_)
        shape_->draw(renderWindow);

    moveButton_->draw(renderWindow);
}

IWindow* ScrollBar::getWindowById(wid_t id)
{
    return (id_ == id ? static_cast<IWindowContainer*>(this) : nullptr);
}

const IWindow* ScrollBar::getWindowById(wid_t id) const
{
    return (id_ == id ? static_cast<const IWindowContainer*>(this) : nullptr);
}

void ScrollBar::addWindow(std::unique_ptr<IWindow> window)
{
    assert(false);
}

void ScrollBar::removeWindow(wid_t id) 
{
    assert(false);
}

vec2i ScrollBar::shrinkPosToBoundaries(vec2i pos, vec2u size) const
{
    assert(size_.x >= size.x);
    assert(size_.y >= size.y);

    vec2i newPos = pos;
    if (newPos.x < pos_.x)
        newPos.x = pos_.x;
    
    if (newPos.y < pos_.y)
        newPos.y = pos_.y;

    if (newPos.x + size.x > pos_.x + size_.x)
        newPos.x = pos_.x + size_.x - size.x;
    
    if (newPos.y + size.y > pos_.y + size_.y)
        newPos.y = pos_.y + size_.y - size.y;

    return newPos;
}

void ScrollBar::setShape(std::unique_ptr<IRectangleShape> shape)
{
    shape_.reset(shape.release());

    shape_->setPosition(pos_);
    shape_->setSize(size_);
}

void ScrollBar::setMoveButton(std::unique_ptr<ScrollBarButton> moveButton)
{
    moveButton_.reset(moveButton.release());

    moveButton_->setParent(static_cast<IWindowContainer*>(this));
}

// Scrollbar button

ScrollBarButton::ScrollBarButton(vec2i pos, vec2u size, wid_t id) : PressButton(pos, size, id)
{
}

void ScrollBarButton::setSize(vec2u size)
{
    for (auto& shape : shapes_)
        shape->setSize(size);

    size_ = size;
}

void ScrollBarButton::move(vec2i delta)
{
    // TODO: move scrollable
    assert(parent_);
    const ScrollBar* parent = dynamic_cast<const ScrollBar*>(parent_); // TODO: change
    assert(parent);

    vec2i newPos = parent->shrinkPosToBoundaries(pos_ + delta, size_);
    for (auto& shape : shapes_)
        shape->setPosition(newPos);

    pos_ = newPos;
}

void ScrollBarButton::setPos(vec2i pos)
{
    // TODO: move scrollable
    vec2i delta = vec2i{pos.x - pos_.x, pos.y - pos_.y};
    move(delta);
}

void ScrollBarButton::draw(IRenderWindow* renderWindow)
{
    shapes_[static_cast<size_t>(state_)]->draw(renderWindow);
}

bool ScrollBarButton::update(const IRenderWindow* renderWindow, const sfm::Event& event)
{
    vec2i mousePos = sfm::Mouse::getPosition(renderWindow);

    bool isPressed = updateIsPressed(event, state_ == State::Pressed, mousePos);
    bool isHovered = checkIsHovered(mousePos);

    state_ = State::Normal;
    state_ = isHovered ? State::Hovered : state_;
    state_ = isPressed ? State::Pressed : state_;

    if (!isPressed)
        return false;

    setPos(mousePos - vec2i{size_.x / 2, size_.y / 2});

    return true;
}

} // namespace ps
