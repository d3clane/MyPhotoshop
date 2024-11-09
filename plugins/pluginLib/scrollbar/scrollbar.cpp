#include "scrollbar.hpp"

#include <cassert>
#include <iostream>

namespace ps
{

namespace
{
    
double calculateScrollButtonRatio(int visibleSize, int fullSize)
{
    return static_cast<double>(visibleSize) / static_cast<double>(fullSize);
}

} // namespace anonymous

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

AScrollBar::AScrollBar(vec2i pos, vec2u size, wid_t id) 
    : AWindowContainer(pos, size, id)
{
}

bool AScrollBar::update(const IRenderWindow* renderWindow, const Event& event)
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

void AScrollBar::draw(IRenderWindow* renderWindow)
{
    if (shape_)
        shape_->draw(renderWindow);

    moveButton_->draw(renderWindow);
}

IWindow* AScrollBar::getWindowById(wid_t id)
{
    return (id_ == id ? static_cast<IWindowContainer*>(this) : nullptr);
}

const IWindow* AScrollBar::getWindowById(wid_t id) const
{
    return (id_ == id ? static_cast<const IWindowContainer*>(this) : nullptr);
}

void AScrollBar::addWindow(std::unique_ptr<IWindow> window)
{
    assert(false);
}

void AScrollBar::removeWindow(wid_t id) 
{
    assert(false);
}

vec2i AScrollBar::shrinkPosToBoundaries(vec2i pos, vec2u size) const
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

void AScrollBar::setShape(std::unique_ptr<IRectangleShape> shape)
{
    shape_.reset(shape.release());

    shape_->setPosition(pos_);
    shape_->setSize(size_);
}

void AScrollBar::setMoveButton(std::unique_ptr<AScrollBarButton> moveButton)
{
    moveButton_.reset(moveButton.release());

    moveButton_->setParent(static_cast<IWindowContainer*>(this));
}

void AScrollBar::setPos(vec2i pos)
{
    pos_ = pos;

    shape_->setPosition(pos_);
}

void AScrollBar::setSize(vec2u size)
{
    size_ = size;

    shape_->setSize(size_);
}

// Scrollbar button

AScrollBarButton::AScrollBarButton(vec2i pos, vec2u size, wid_t id) : PressButton(pos, size, id)
{
}

void AScrollBarButton::setSize(vec2u size)
{
    for (auto& shape : shapes_)
        shape->setSize(size);

    size_ = size;
}

void AScrollBarButton::move(vec2i delta)
{
    assert(scrollable_);

    assert(parent_);
    const AScrollBar* parent = dynamic_cast<const AScrollBar*>(parent_); // TODO: change
    assert(parent);

    vec2i newPos = parent->shrinkPosToBoundaries(pos_ + delta, size_);
    for (auto& shape : shapes_)
        shape->setPosition(newPos);

    vec2u parentSize = parent->getSize();
    if (parentSize.x != 0)
    {
        scroll_.x = static_cast<float>(newPos.x - zeroScrollPos_.x) / static_cast<float>(parentSize.x - size_.x);
        scroll_.y = static_cast<float>(newPos.y - zeroScrollPos_.y) / static_cast<float>(parentSize.y - size_.y);
    }

    pos_ = newPos;

    scrollable_->setScroll(scroll_);
}

void AScrollBarButton::setPos(vec2i pos)
{
    vec2i delta = vec2i{pos.x - pos_.x - size_.x / 2, pos.y - pos_.y - size_.y / 2};
    move(delta);
}

void AScrollBarButton::draw(IRenderWindow* renderWindow)
{
    shapes_[static_cast<size_t>(state_)]->draw(renderWindow);
}

bool AScrollBarButton::update(const IRenderWindow* renderWindow, const sfm::Event& event)
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

void AScrollBarButton::setScrollable(IScrollable* scrollable)
{
    scrollable_ = scrollable;
}

// ScrollbarX implementation

ScrollBarX::ScrollBarX(vec2i pos, vec2u size, wid_t id) : AScrollBar(pos, size, id)
{ 
    moveButton_ = std::make_unique<ScrollBarButtonX>(pos, size, id);
}

void ScrollBarX::updatePos()
{
    assert(parent_);

    vec2i parentPos  = parent_->getPos();
    vec2u parentSize = parent_->getSize();
    
    setPos(vec2i{ parentPos.x, parentPos.y + parentSize.y });
}

void ScrollBarX::updateSize()
{
    assert(parent_);

    vec2u parentSize = parent_->getSize();

    static const size_t prettyCoeff = 40;
    static const size_t minYSize = 5;

    setSize(vec2u{parentSize.x, std::max(minYSize, parentSize.y / prettyCoeff)});
}

// ScrollbarY implementation

ScrollBarY::ScrollBarY(vec2i pos, vec2u size, wid_t id) : AScrollBar(pos, size, id)
{ 
    moveButton_ = std::make_unique<ScrollBarButtonY>(pos, size, id);
}

void ScrollBarY::updatePos()
{
    assert(parent_);

    vec2i parentPos = parent_->getPos();
    vec2u parentSize = parent_->getSize();
    
    setPos(vec2i{ parentPos.x + parentSize.x, parentPos.y });
}

void ScrollBarY::updateSize()
{
    assert(parent_);

    vec2u parentSize = parent_->getSize();

    static const size_t prettyCoeff = 40;
    static const size_t minYSize = 5;

    setSize(vec2u{std::max(minYSize, parentSize.x / prettyCoeff), parentSize.y});
}

// ScrollBarButtonX implementation

ScrollBarButtonX::ScrollBarButtonX(vec2i pos, vec2u size, wid_t id) : AScrollBarButton(pos, size, id) 
{
}

// TODO: copy paste
void ScrollBarButtonX::updateZeroScrollPos()
{
    assert(parent_);

    // TODO: set pos correctly by scroll
    if (scroll_.x > 0 || scroll_.y > 0)
        return;

    assert(parent_);
    const AScrollBar* parent = dynamic_cast<const AScrollBar*>(parent_);
    assert(parent);

    vec2i newPos = parent->shrinkPosToBoundaries(parent_->getPos() , size_);

    for (auto& shape : shapes_)
        shape->setPosition(newPos);

    zeroScrollPos_ = newPos;
    pos_ = newPos;
}

void ScrollBarButtonX::updateSize()
{
    assert(parent_);
    assert(scrollable_);

    vec2u parentSize = parent_->getSize();

    double xRatio = calculateScrollButtonRatio(scrollable_->getVisibleSize().x, scrollable_->getFullSize().x);

    setSize(vec2u{static_cast<unsigned>(parentSize.x * xRatio), parentSize.y});
}

// ScrollBarButtonY implementation

ScrollBarButtonY::ScrollBarButtonY(vec2i pos, vec2u size, wid_t id) : AScrollBarButton(pos, size, id) 
{
}


void ScrollBarButtonY::updateZeroScrollPos()
{
    assert(parent_);

    // TODO: set pos correctly by scroll
    if (scroll_.x > 0 || scroll_.y > 0)
        return;

    assert(parent_);
    const AScrollBar* parent = dynamic_cast<const AScrollBar*>(parent_);
    assert(parent);

    vec2i newPos = parent->shrinkPosToBoundaries(parent_->getPos() , size_);

    for (auto& shape : shapes_)
        shape->setPosition(newPos);

    zeroScrollPos_ = newPos;
    pos_ = newPos;
}

void ScrollBarButtonY::updateSize()
{
    assert(parent_);
    assert(scrollable_);

    vec2u parentSize = parent_->getSize();

    double yRatio = calculateScrollButtonRatio(scrollable_->getVisibleSize().y, scrollable_->getFullSize().y);

    setSize(vec2u{parentSize.x, static_cast<unsigned>(parentSize.y * yRatio)});
}

} // namespace ps
