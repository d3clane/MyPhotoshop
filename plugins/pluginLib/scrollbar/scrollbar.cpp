#include "scrollbar.hpp"

#include <cassert>
#include <iostream>

namespace ps
{

namespace
{
    
double calculateScrollButtonRatio(unsigned visibleSize, unsigned fullSize)
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

AScrollBar::AScrollBar(vec2i pos, vec2u size, wid_t id) : id_(id), pos_(pos), size_(size)
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
    bool isHovered = checkIsHovered(mousePos, pos_, size_);
    bool isPressed = (event.type == Event::MouseButtonPressed);

    if (!isHovered || !isPressed)
        return false;

    moveButton_->setPos(mousePos - vec2i{static_cast<int>(size_.x / 2), static_cast<int>(size_.y / 2)});
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

bool AScrollBar::isWindowContainer() const
{
    return true;
}

wid_t AScrollBar::getId() const
{
    return id_;
}

vec2i AScrollBar::getPos() const
{
    return pos_;
}

vec2u AScrollBar::getSize() const
{
    return size_;
}

void AScrollBar::setParent(const IWindow* parent)
{
    parent_ = parent;
}

void AScrollBar::forceActivate()
{
    isActive_ = true;
}

void AScrollBar::forceDeactivate()
{
    isActive_ = false;
}

bool AScrollBar::isActive() const
{
    return isActive_;
}

void AScrollBar::addWindow(std::unique_ptr<IWindow> /* window */)
{
    assert(false);
}

void AScrollBar::removeWindow(wid_t /* id */) 
{
    assert(false);
}

vec2i AScrollBar::shrinkPosToBoundaries(vec2i pos, vec2u size) const
{
    assert(size_.x >= size.x);
    assert(size_.y >= size.y);

    vec2i newPos = pos;
    vec2i intSize_ = vec2i{static_cast<int>(size_.x), static_cast<int>(size_.y)};
    vec2i intSize  = vec2i{static_cast<int>(size.x ), static_cast<int>(size.y )};

    newPos.x = newPos.x < pos_.x ? pos_.x : newPos.x;
    newPos.y = newPos.y < pos_.y ? pos_.y : newPos.y;
    newPos.x = newPos.x + intSize.x > pos_.x + intSize_.x ? pos_.x + intSize_.x - intSize.x : newPos.x;
    newPos.y = newPos.y + intSize.y > pos_.y + intSize_.y ? pos_.y + intSize_.y - intSize.y : newPos.y;

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

void AScrollBarButton::move(vec2d delta)
{
    assert(scrollable_);

    assert(parent_);
    const AScrollBar* parent = static_cast<const AScrollBar*>(parent_); // TODO: change
    assert(parent);

    vec2u parentSize = parent->getSize();
    
    vec2f scrollNow = scrollable_->getScroll(); 

    if (parentSize.x != 0)
    {
        scroll_.x += static_cast<float>(delta.x) / static_cast<float>(parentSize.x - size_.x);
        scroll_.y += static_cast<float>(delta.y) / static_cast<float>(parentSize.y - size_.y);
    }

    scroll_.x = canScrollX_ ? scroll_.x : scrollNow.x;
    scroll_.y = canScrollY_ ? scroll_.y : scrollNow.y;

    vec2i newPos = parent->shrinkPosToBoundaries(
        zeroScrollPos_ + vec2i{static_cast<int>(scroll_.x * static_cast<float>(parentSize.x - size_.x)),
                               static_cast<int>(scroll_.y * static_cast<float>(parentSize.y - size_.y))}, 
        size_
    );

    for (auto& shape : shapes_)
        shape->setPosition(newPos);

    pos_ = newPos;

    scrollable_->setScroll(scroll_);
}

void AScrollBarButton::setPos(vec2i pos)
{
    vec2d delta = vec2d{pos.x - pos_.x, pos.y - pos_.y};
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
        move(vec2d{mousePos.x - pressPos_.x, mousePos.y - pressPos_.y});

    pressPos_ = mousePos;

    return true;
}

void AScrollBarButton::setScrollable(IScrollableWindow* scrollable)
{
    scrollable_ = scrollable;
}

const IScrollableWindow* AScrollBarButton::getScrollable() const 
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

    setPos(vec2i{ parentPos.x, parentPos.y + static_cast<int>(parentSize.y)});
}

void ScrollBarX::updateSize()
{
    assert(parent_);
    vec2u parentSize = parent_->getSize();

    static const unsigned prettyCoeff = 40;
    static const unsigned minYSize = 5;

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
    
    setPos(vec2i{ parentPos.x + static_cast<int>(parentSize.x), parentPos.y });
}

void ScrollBarY::updateSize()
{
    assert(parent_);

    vec2u parentSize = parent_->getSize();

    static const unsigned prettyCoeff = 40;
    static const unsigned minYSize = 5;

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
    const AScrollBar* parent = static_cast<const AScrollBar*>(parent_);
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
    const AScrollBar* parent = static_cast<const AScrollBar*>(parent_);
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

    if (event.type != Event::MouseWheelScrolled)
        return false;

    ScrollBarButtonX* scrollBarButtonX = static_cast<ScrollBarButtonX*>(scrollBarX_->getMoveButton());
    ScrollBarButtonY* scrollBarButtonY = static_cast<ScrollBarButtonY*>(scrollBarY_->getMoveButton());
    assert(scrollBarButtonX);
    assert(scrollBarButtonY);

    const IScrollableWindow* scrollable = scrollBarButtonX->getScrollable();
    assert(scrollable);

    if (!ps::checkIsHovered(vec2i{event.mouseWheel.x, event.mouseWheel.y}, 
                            scrollable->getPos(), scrollable->getSize()))
    {
    #if 0
        promisedScroll_ = {0, 0};
    #endif

        return false;
    }

    static const double scrollSpeed = -1;

    vec2d deltaMove;
    if (event.mouseWheel.wheel == Mouse::Wheel::Vertical)
        deltaMove = {0, event.mouseWheel.delta * scrollSpeed};
    else
        deltaMove = {event.mouseWheel.delta * scrollSpeed, 0};

    scrollBarButtonX->move(deltaMove);
    scrollBarButtonY->move(deltaMove);
    
#if 0
    updatePromisedScroll(event);
    proceedPromisedScroll(scrollBarButtonX, scrollBarButtonY);
#endif

    return true;
}

void ScrollBarsXYManager::draw(IRenderWindow* renderWindow)
{
    scrollBarX_->draw(renderWindow);
    scrollBarY_->draw(renderWindow);
}

void ScrollBarsXYManager::addWindow(std::unique_ptr<IWindow> /* window */)
{
    assert(false);
}

void ScrollBarsXYManager::removeWindow(wid_t /* id */)
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

bool ScrollBarsXYManager::isWindowContainer() const { return true; }

wid_t ScrollBarsXYManager::getId() const { return id_; }

vec2i ScrollBarsXYManager::getPos() const { return pos_; }
vec2u ScrollBarsXYManager::getSize() const { return size_; }

void ScrollBarsXYManager::setParent(const IWindow* parent) { parent_ = parent; }

void ScrollBarsXYManager::forceActivate() { isActive_ = true; }
void ScrollBarsXYManager::forceDeactivate() { isActive_ = false; }

bool ScrollBarsXYManager::isActive() const { return isActive_; }

#if 0
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

    float evenScrollX = std::ceil(scrollBarX_->getSize().x / 300.f);
    float evenScrollY = std::ceil(scrollBarY_->getSize().y / 150.f);

    float directionX = std::signbit(promisedScroll_.x) ? -1.f : 1.f;
    float directionY = std::signbit(promisedScroll_.y) ? -1.f : 1.f;

    evenScrollX = directionX * std::min(std::abs(promisedScroll_.x), evenScrollX);
    evenScrollY = directionY * std::min(std::abs(promisedScroll_.y), evenScrollY);

    vec2f scrollInPixels_float = vec2f{evenScrollX, evenScrollY};
    vec2i scrollInPixels = vec2i{static_cast<int>(scrollInPixels_float.x), 
                                 static_cast<int>(scrollInPixels_float.y)};
    
    //scrollBarButtonX->move(scrollInPixels);
    //scrollBarButtonY->move(scrollInPixels);

    promisedScroll_ -= vec2f{static_cast<float>(scrollInPixels.x), static_cast<float>(scrollInPixels.y)};
}
#endif

} // namespace ps
