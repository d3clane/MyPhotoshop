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

    moveButton_->setPos(mousePos);
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

AScrollBarButton* AScrollBar::getMoveButton()
{
    return moveButton_.get();
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
    
    vec2f scrollNow = scrollable_->getScroll();
    if (parentSize.x != 0)
    {
        scroll_.x = static_cast<float>(newPos.x - zeroScrollPos_.x) / static_cast<float>(parentSize.x - size_.x);
        scroll_.y = static_cast<float>(newPos.y - zeroScrollPos_.y) / static_cast<float>(parentSize.y - size_.y);
    }

    scroll_.x = canScrollX_ ? scroll_.x : scrollNow.x;
    scroll_.y = canScrollY_ ? scroll_.y : scrollNow.y;    

    pos_ = newPos;

    scrollable_->setScroll(scroll_);
}

void AScrollBarButton::setPos(vec2i pos)
{
    vec2i delta = vec2i{pos.x - pos_.x - size_.x / 2, pos.y - pos_.y - size_.y / 2};
    move(delta);
}

void AScrollBarButton::setState(State state)
{
    stateToSet_ = state;
    needToSetState_ = true;
}

void AScrollBarButton::draw(IRenderWindow* renderWindow)
{
    shapes_[static_cast<size_t>(state_)]->draw(renderWindow);
}

void AScrollBarButton::setStateFromOutside(const IRenderWindow* renderWindow)
{
    if (!needToSetState_)
        return;
    
    if (stateToSet_ != State::Pressed)
        return;

    if (state_ == State::Pressed)
        return;
    
    state_ = State::Pressed;
    pressPos_ = sfm::Mouse::getPosition(renderWindow);

    needToSetState_ = false;
}

bool AScrollBarButton::update(const IRenderWindow* renderWindow, const sfm::Event& event)
{
    updateZeroScrollPos();
    updateSize();

    setStateFromOutside(renderWindow);
    vec2i mousePos = sfm::Mouse::getPosition(renderWindow);

    bool wasPressed = state_ == State::Pressed;
    bool isPressed = updateIsPressed(event, wasPressed, mousePos);
    bool isHovered = checkIsHovered(mousePos);

    state_ = State::Normal;
    state_ = isHovered ? State::Hovered : state_;
    state_ = isPressed ? State::Pressed : state_;

    if (!isPressed)
        return false;

    if (wasPressed)
        move(mousePos - pressPos_);

    pressPos_ = mousePos;

    return true;
}

void AScrollBarButton::setScrollable(AScrollableWindow* scrollable)
{
    scrollable_ = scrollable;
}

const AScrollableWindow* AScrollBarButton::getScrollable() const 
{
    return scrollable_;
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
    canScrollX_ = true;
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
    canScrollY_ = true;
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

// scroll bar xy manager implementation

ScrollBarsXYManager::ScrollBarsXYManager(
    std::unique_ptr<ScrollBarX> scrollBarX, std::unique_ptr<ScrollBarY> scrollBarY
) : scrollBarX_(std::move(scrollBarX)), scrollBarY_(std::move(scrollBarY))
{
}

bool ScrollBarsXYManager::update(const IRenderWindow* renderWindow, const Event& event)
{
    scrollBarX_->update(renderWindow, event);
    scrollBarY_->update(renderWindow, event);

    ScrollBarButtonX* scrollBarButtonX = static_cast<ScrollBarButtonX*>(scrollBarX_->getMoveButton());
    ScrollBarButtonY* scrollBarButtonY = static_cast<ScrollBarButtonY*>(scrollBarY_->getMoveButton());
    assert(scrollBarButtonX);
    assert(scrollBarButtonY);

    const AScrollableWindow* scrollable = scrollBarButtonX->getScrollable();
    assert(scrollable);

    if (!ps::checkIsHovered(vec2i{event.mouseWheel.x, event.mouseWheel.y}, 
                            scrollable->getPos(), scrollable->getSize()))
    {
        promisedScroll_ = {0, 0};
        return false;
    }

    updatePromisedScroll(event);
    proceedPromisedScroll(scrollBarButtonX, scrollBarButtonY);

    return true;
}

void ScrollBarsXYManager::draw(IRenderWindow* renderWindow)
{
    scrollBarX_->draw(renderWindow);
    scrollBarY_->draw(renderWindow);
}

void ScrollBarsXYManager::addWindow(std::unique_ptr<IWindow> window)
{
    assert(false);
}

void ScrollBarsXYManager::removeWindow(wid_t id)
{
    assert(false);
}

IWindow* ScrollBarsXYManager::getWindowById(wid_t id)
{
    if (id == id_)
        return static_cast<IWindowContainer*>(this);

    if (scrollBarX_->getWindowById(id))
        return scrollBarX_->getWindowById(id);
    
    if (scrollBarY_->getWindowById(id))
        return scrollBarY_->getWindowById(id);

    return nullptr;
}

const IWindow* ScrollBarsXYManager::getWindowById(wid_t id) const
{
    if (id == id_)
        return static_cast<const IWindowContainer*>(this);
    
    if (scrollBarX_->getWindowById(id))
        return scrollBarX_->getWindowById(id);
    
    if (scrollBarY_->getWindowById(id))
        return scrollBarY_->getWindowById(id);

    return nullptr;
}

void ScrollBarsXYManager::updatePromisedScroll(const Event& event)
{
    if (event.type != Event::MouseWheelScrolled)
        return;

    static const double scrollSpeed = -1;

    if (event.mouseWheel.wheel == Mouse::Wheel::Vertical)
        promisedScroll_.y += event.mouseWheel.delta * scrollSpeed;
    else
        promisedScroll_.x += event.mouseWheel.delta * scrollSpeed;

    if (std::abs(promisedScroll_.x) > std::abs(promisedScroll_.y))
        promisedScroll_.y = 0;
    if (std::abs(promisedScroll_.y) > std::abs(promisedScroll_.x))
        promisedScroll_.x = 0;
}

void ScrollBarsXYManager::proceedPromisedScroll(ScrollBarButtonX* scrollBarButtonX, 
                                                ScrollBarButtonY* scrollBarButtonY)
{
    if (promisedScroll_.x == 0.0 && promisedScroll_.y == 0.0)
        return;

    static const double kScrollSmoothness = 0.2;

    double scrollSmoothness = kScrollSmoothness;
    static const double minScrollDeltaInPixels = 2;
    if (std::abs(promisedScroll_.x) < minScrollDeltaInPixels)
        scrollSmoothness = 1;

    vec2f scrollInPixels_float = promisedScroll_ * scrollSmoothness;
    vec2i scrollInPixels = vec2i{static_cast<int>(scrollInPixels_float.x), 
                                 static_cast<int>(scrollInPixels_float.y)};
    
    scrollBarButtonX->move(scrollInPixels);
    scrollBarButtonY->move(scrollInPixels);

    promisedScroll_ -= vec2f{static_cast<float>(scrollInPixels.x), static_cast<float>(scrollInPixels.y)};
}

} // namespace ps
