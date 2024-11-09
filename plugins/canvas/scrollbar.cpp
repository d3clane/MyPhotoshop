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

void PressButton::setState(State state)
{
    state_ = state;
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

    updatePos();
    updateSize();
    
    bool eventUsed = moveButton_->update(renderWindow, event);
    if (eventUsed)
        return true;

    vec2i mousePos = sfm::Mouse::getPosition(renderWindow);
    bool isHovered = checkIsHovered(mousePos);
    bool isPressed = (event.type == Event::MouseButtonPressed);

    if (!isHovered || !isPressed)
        return false;

    moveButton_->setPos(shrinkPosToBoundaries(mousePos, moveButton_->getSize()));
    moveButton_->setState(PressButton::State::Pressed);

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
    newPos.x = newPos.x < pos_.x ? pos_.x : newPos.x;
    newPos.y = newPos.y < pos_.y ? pos_.y : newPos.y;
    newPos.x = newPos.x + size.x > pos_.x + size_.x ? pos_.x + size_.x - size.x : newPos.x;
    newPos.y = newPos.y + size.y > pos_.y + size_.y ? pos_.y + size_.y - size.y : newPos.y;

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

void ScrollBar::setPos(vec2i pos)
{
    pos_ = pos;

    shape_->setPosition(pos_);
}

void ScrollBar::setSize(vec2u size)
{
    size_ = size;

    shape_->setSize(size_);
}

void ScrollBar::updatePos()
{
    assert(parent_);

    vec2i parentPos = parent_->getPos();
    vec2u parentSize = parent_->getSize();
    
    setPos(vec2i{ parentPos.x, parentPos.y + parentSize.y });
}

void ScrollBar::updateSize()
{
    assert(parent_);

    vec2u parentSize = parent_->getSize();

    static const size_t prettyCoeff = 40;
    static const size_t minYSize = 5;

    setSize(vec2u{parentSize.x, std::max(minYSize, parentSize.y / prettyCoeff)});
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
    assert(scrollable_);

    assert(parent_);
    const ScrollBar* parent = dynamic_cast<const ScrollBar*>(parent_); // TODO: change
    assert(parent);

    vec2i newPos = parent->shrinkPosToBoundaries(pos_ + delta, size_);
    for (auto& shape : shapes_)
        shape->setPosition(newPos);

    // TODO: actually easy to fix to create not only horizontal one

    vec2u parentSize = parent->getSize();
    if (parentSize.x != 0)
        scroll_ = static_cast<float>(newPos.x - zeroScrollPos_.x) / static_cast<float>(parentSize.x - size_.x);

    pos_ = newPos;

    scrollable_->setScroll(vec2f{scroll_, 0}); // TODO: only horizontal 
}

void ScrollBarButton::setPos(vec2i pos)
{
    vec2i delta = vec2i{pos.x - pos_.x - size_.x / 2, pos.y - pos_.y - size_.y / 2};
    move(delta);
}

void ScrollBarButton::draw(IRenderWindow* renderWindow)
{
    shapes_[static_cast<size_t>(state_)]->draw(renderWindow);
}

bool ScrollBarButton::update(const IRenderWindow* renderWindow, const sfm::Event& event)
{
    updateZeroScrollPos();
    updateSize();

    vec2i mousePos = sfm::Mouse::getPosition(renderWindow);

    bool isPressed = updateIsPressed(event, state_ == State::Pressed, mousePos);
    bool isHovered = checkIsHovered(mousePos);

    state_ = State::Normal;
    state_ = isHovered ? State::Hovered : state_;
    state_ = isPressed ? State::Pressed : state_;

    if (!isPressed)
        return false;

    setPos(mousePos);

    return true;
}

void ScrollBarButton::setScrollable(IScrollable* scrollable)
{
    scrollable_ = scrollable;
}

// TODO: need to have fields like beginPos for scrolling. On updating pos need to actually change INIT POS, not pos, error will be fixed
void ScrollBarButton::updateZeroScrollPos()
{
    assert(parent_);

    // TODO: set pos correctly by scroll
    if (scroll_ > 0)
        return;

    assert(parent_);
    const ScrollBar* parent = dynamic_cast<const ScrollBar*>(parent_); // TODO: change
    assert(parent);

    vec2i newPos = parent->shrinkPosToBoundaries(parent_->getPos() , size_);

    for (auto& shape : shapes_)
        shape->setPosition(newPos);

    zeroScrollPos_ = newPos;
    pos_ = newPos;
}

void ScrollBarButton::updateSize()
{
    assert(parent_);
    assert(scrollable_);

    vec2u parentSize = parent_->getSize();

    double xRatio = static_cast<double>(scrollable_->getVisibleSize().x) / 
                    static_cast<double>(scrollable_->getFullSize   ().x);

    setSize(vec2u{static_cast<unsigned>(parentSize.x * xRatio), parentSize.y});
}

} // namespace ps
