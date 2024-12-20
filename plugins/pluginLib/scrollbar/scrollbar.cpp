#include "scrollbar.hpp"
#include "pluginLib/actions/actions.hpp"

#include <cassert>
#include <iostream>
#include <algorithm>

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

#if 0
static void configureSprite(ISprite* sprite, vec2u size, vec2i pos, 
                     SpriteInfo& leftBoundary, SpriteInfo& rightBoundary)
{
    vec2u deltaLeft = leftBoundary.sprite->getSize();
    vec2u deltaRight = rightBoundary.sprite->getSize();

    sprite->setScale(1.f, 1.f);
    sprite->setScale(
        static_cast<float>(size.x - deltaLeft.x - deltaRight.x) / static_cast<float>(sprite->getSize().x), 
        static_cast<float>(size.y - deltaLeft.y - deltaRight.y) / static_cast<float>(sprite->getSize().y));

    sprite->setPosition((float)pos.x + (float)deltaLeft.x, (float)pos.y + (float)deltaLeft.y);
}
#endif

void PressButton::setInsideSprite(SpriteInfo shape, State state)
{
    auto& myShape = sprites_[static_cast<size_t>(state)];
    myShape = std::move(shape);
    configureSprites();
}

void PressButton::setLeftBoundarySprite(SpriteInfo sprite)
{
    leftBoundary_ = std::move(sprite);
    configureSprites();
}

void PressButton::setRightBoundarySprite(SpriteInfo sprite)
{
    rightBoundary_ = std::move(sprite);
    configureSprites();
}

// Scrollbar implementation

AScrollBar::AScrollBar(vec2i pos, vec2u size, vec2i deltaFromPos, vec2u deltaFromSize, wid_t id)
    : id_(id), pos_(pos), size_(size), moveButton_(nullptr),
      deltaFromPos_(deltaFromPos), deltaFromSize_(deltaFromSize)
{
}

std::unique_ptr<IAction> AScrollBar::createAction(const IRenderWindow* renderWindow, 
                                                  const Event& event)
{
    return std::make_unique<UpdateCallbackAction<AScrollBar>>(*this, renderWindow, event);
}

bool AScrollBar::update(const IRenderWindow* renderWindow, const Event& event)
{
    if (!isActive_)
        return false;

#if 0
    fprintf(stderr, "----\n");
    fprintf(stderr, "MY SIZE - (%u, %u)\n", size_.x, size_.y);
    fprintf(stderr, "MY POS - (%d, %d)\n", pos_.x, pos_.y);
    fprintf(stderr, "MOVE BUTTON POS - (%d, %d)\n", moveButton_->getPos().x, moveButton_->getPos().y);
    fprintf(stderr, "MOVE BUTTON SIZE - (%u, %u)\n", moveButton_->getSize().x, moveButton_->getSize().y);
#endif

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
    if (sprite_.sprite)
        sprite_.sprite->draw(renderWindow);

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

void AScrollBar::setSprite(SpriteInfo sprite)
{
    sprite_ = std::move(sprite);
    
    sprite_.sprite->setPosition(vec2f{static_cast<float>(pos_.x), static_cast<float>(pos_.y)});
    
    //assert(sprite_.sprite->getSize().x == size_.x && sprite_.sprite->getSize().y == size_.y);
}

void AScrollBar::setMoveButton(std::unique_ptr<AScrollBarButton> moveButton)
{
    moveButton_.reset(moveButton.release());
    moveButton_->setParent(static_cast<IWindowContainer*>(this));

    moveButton_->setZeroSCrollPos(pos_ + deltaFromPos_);
}

AScrollBarButton* AScrollBar::getMoveButton()
{
    return moveButton_.get();
}

void AScrollBar::setPos(const vec2i& pos)
{
    pos_ = pos;

    sprite_.sprite->setPosition(vec2f{static_cast<float>(pos_.x), static_cast<float>(pos_.y)});
}

void AScrollBar::setSize(const vec2u& size)
{
    size_ = size;

    assert(0);
}

// Scrollbar button

AScrollBarButton::AScrollBarButton(vec2i pos, vec2u size, wid_t id) : PressButton(pos, size, id)
{
}

void AScrollBarButton::setSize(const vec2u& size)
{
    size_ = size;

    configureSprites();
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

    scroll_.x = std::clamp(scroll_.x, 0.f, 1.f);
    scroll_.y = std::clamp(scroll_.y, 0.f, 1.f);

    vec2i newPos = parent->shrinkPosToBoundaries(
        zeroScrollPos_ + vec2i{static_cast<int>(scroll_.x * static_cast<float>(parentSize.x - size_.x)),
                               static_cast<int>(scroll_.y * static_cast<float>(parentSize.y - size_.y))}, 
        size_
    );

    configureSprites();
    pos_ = newPos;

    scrollable_->setScroll(scroll_);
}

void AScrollBarButton::setPos(const vec2i& pos)
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
    sprites_[static_cast<size_t>(state_)].sprite->draw(renderWindow);
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

std::unique_ptr<IAction> AScrollBarButton::createAction(const IRenderWindow* renderWindow, 
                                                        const Event& event)
{
    return std::make_unique<UpdateCallbackAction<AScrollBarButton>>(*this, renderWindow, event);
}

bool AScrollBarButton::update(const IRenderWindow* renderWindow, const sfm::Event& event)
{
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

void AScrollBarButton::setScrollable(IScrollable* scrollable)
{
    scrollable_ = scrollable;
}

const IScrollable* AScrollBarButton::getScrollable() const 
{
    return scrollable_;
}

void AScrollBarButton::setZeroSCrollPos(vec2i pos)
{
    zeroScrollPos_ = pos;

    fprintf(stderr, "SETTING ZERO SCROLL POS TO (%d, %d)\n", zeroScrollPos_.x, zeroScrollPos_.y);

    configureSprites();
}

// ScrollbarX implementation

ScrollBarX::ScrollBarX(vec2i pos, vec2u size, vec2i deltaFromPos, vec2u deltaFromSize, wid_t id) 
    : AScrollBar(pos, size, deltaFromPos, deltaFromSize, id)
{ 
    moveButton_ = std::make_unique<ScrollBarButtonX>(pos, size, id);
}

vec2i ScrollBarX::shrinkPosToBoundaries(vec2i pos, vec2u size) const
{
    vec2i leftBoundary = vec2i{pos_.x + deltaFromPos_.x, pos_.y + deltaFromPos_.y};
    vec2i rightBoundary = vec2i{pos_.x + (int)size_.x - (int)deltaFromSize_.x - (int)size.x, pos_.y + deltaFromPos_.y};

#if 0
    fprintf(stderr, "POS BEFORE SHRINKING - (%d, %d)\n", pos.x, pos.y);
    fprintf(stderr, "LEFT BOUNDARY - (%d, %d)\n", leftBoundary.x, leftBoundary.y);
    fprintf(stderr, "RIGHT BOUNDARY - (%d, %d)\n", rightBoundary.x, rightBoundary.y);
    fprintf(stderr, "SIZE - %u %u\n", size.x, size.y);
    fprintf(stderr, "SIZE_ - %u %u\n", size_.x, size_.y);
#endif

    if (pos.x < leftBoundary.x)
        pos.x = leftBoundary.x;
    
    pos.y = pos_.y + deltaFromPos_.y;
    
    if (pos.x > rightBoundary.x)
        pos.x = rightBoundary.x;
#if 0
    fprintf(stderr, "POS AFTER SHRINKING - (%d, %d)\n", pos.x, pos.y);
#endif

    return pos;
}

void ScrollBarButtonX::updateSize()
{
    assert(parent_);
    assert(scrollable_);

    vec2u parentSize = parent_->getSize();

    double xRatio = calculateScrollButtonRatio(scrollable_->getVisibleSize().x, scrollable_->getFullSize().x);

    setSize(vec2u{static_cast<unsigned>(parentSize.x * xRatio), size_.y});
}

// ScrollbarY implementation

ScrollBarY::ScrollBarY(vec2i pos, vec2u size, vec2i deltaFromPos, vec2u deltaFromSize, wid_t id) 
    : AScrollBar(pos, size, deltaFromPos, deltaFromSize, id)
{ 
    moveButton_ = std::make_unique<ScrollBarButtonY>(pos, size, id);
}

vec2i ScrollBarY::shrinkPosToBoundaries(vec2i pos, vec2u size) const
{
    vec2i leftBoundary = vec2i{pos_.x + deltaFromPos_.x, pos_.y + deltaFromPos_.y};
    vec2i rightBoundary = vec2i{pos_.x + deltaFromPos_.x, pos_.y + (int)size_.y - (int)deltaFromSize_.y - (int)size.y};

#if 0
    fprintf(stderr, "POS BEFORE SHRINKING - (%d, %d)\n", pos.x, pos.y);
    fprintf(stderr, "LEFT BOUNDARY - (%d, %d)\n", leftBoundary.x, leftBoundary.y);
    fprintf(stderr, "RIGHT BOUNDARY - (%d, %d)\n", rightBoundary.x, rightBoundary.y);
    fprintf(stderr, "SIZE - %u %u\n", size.x, size.y);
    fprintf(stderr, "SIZE_ - %u %u\n", size_.x, size_.y);
#endif

    if (pos.y < leftBoundary.y)
        pos.y = leftBoundary.y;
    
    pos.x = pos_.x + deltaFromPos_.x;
    
    if (pos.y > rightBoundary.y)
        pos.y = rightBoundary.y;

#if 0
    fprintf(stderr, "POS AFTER SHRINKING - (%d, %d)\n", pos.x, pos.y);
#endif

    return pos;
}

// ScrollBarButtonX implementation

ScrollBarButtonX::ScrollBarButtonX(vec2i pos, vec2u size, wid_t id) : AScrollBarButton(pos, size, id) 
{
    canScrollX_ = true;
}

void ScrollBarButtonX::configureSprites()
{
    if (!leftBoundary_.sprite || !rightBoundary_.sprite)
        return;

#if 0
    fprintf(stderr, "POS - %d %d\n", pos_.x, pos_.y);
#endif

    leftBoundary_.sprite->setPosition((float)pos_.x, (float)pos_.y);

    rightBoundary_.sprite->setPosition((float)pos_.x + (float)size_.x - (float)rightBoundary_.sprite->getSize().x, 
                                            (float)pos_.y);

    vec2u deltaLeft = leftBoundary_.sprite->getSize();
    vec2u deltaRight = rightBoundary_.sprite->getSize();

    for (auto& sprite : sprites_)
    {
        if (!sprite.sprite)
            continue;

        sprite.sprite->setScale(1.f, 1.f);
        sprite.sprite->setScale(
            static_cast<float>(size_.x - deltaLeft.x - deltaRight.x) / static_cast<float>(sprite.sprite->getSize().x), 
            1.f);
        
        sprite.sprite->setPosition((float)pos_.x + (float)deltaLeft.x, pos_.y);
    }
}

// ScrollBarButtonY implementation

ScrollBarButtonY::ScrollBarButtonY(vec2i pos, vec2u size, wid_t id) : AScrollBarButton(pos, size, id) 
{
    canScrollY_ = true;
}

void ScrollBarButtonY::configureSprites()
{
    if (!leftBoundary_.sprite || !rightBoundary_.sprite)
        return;

    leftBoundary_.sprite->setPosition((float)pos_.x, (float)pos_.y);
    rightBoundary_.sprite->setPosition((float)pos_.x,
                                       (float)pos_.y + (float)size_.y - (float)rightBoundary_.sprite->getSize().y);

    vec2u deltaLeft = leftBoundary_.sprite->getSize();
    vec2u deltaRight = rightBoundary_.sprite->getSize();

    for (auto& sprite : sprites_)
    {
        if (!sprite.sprite)
            continue;

        sprite.sprite->setScale(1.f, 1.f);
        sprite.sprite->setScale(
            1.f,
            static_cast<float>(size_.y - deltaLeft.y - deltaRight.y) / static_cast<float>(sprite.sprite->getSize().y));
        
        //assert(sprite.sprite->getSize().x == size_.x);

        sprite.sprite->setPosition((float)pos_.x, (float)pos_.y + (float)deltaLeft.y);
    }
}

void ScrollBarButtonY::updateSize()
{
    assert(parent_);
    assert(scrollable_);

    vec2u parentSize = parent_->getSize();

    double yRatio = calculateScrollButtonRatio(scrollable_->getVisibleSize().y, scrollable_->getFullSize().y);

    setSize(vec2u{size_.x, static_cast<unsigned>(parentSize.y * yRatio)});
}

// scroll bar xy manager implementation

ScrollBarsXYManager::ScrollBarsXYManager(
    std::unique_ptr<ScrollBarX> scrollBarX, std::unique_ptr<ScrollBarY> scrollBarY,
    std::unique_ptr<ICanScrollAction> canScrollAction
) : scrollBarX_(std::move(scrollBarX)), scrollBarY_(std::move(scrollBarY)), 
    canScrollAction_(std::move(canScrollAction))
{
}

std::unique_ptr<IAction> ScrollBarsXYManager::createAction(const IRenderWindow* renderWindow,
                                                           const Event& event)
{
    return std::make_unique<UpdateCallbackAction<ScrollBarsXYManager>>(*this, renderWindow, event);
}

bool ScrollBarsXYManager::update(const IRenderWindow* renderWindow, const Event& event)
{
    if (scrollBarX_) scrollBarX_->update(renderWindow, event);
    if (scrollBarY_) scrollBarY_->update(renderWindow, event);

    if (event.type != Event::MouseWheelScrolled)
        return false;


    ScrollBarButtonX* scrollBarButtonX = static_cast<ScrollBarButtonX*>(scrollBarX_->getMoveButton());
    ScrollBarButtonY* scrollBarButtonY = static_cast<ScrollBarButtonY*>(scrollBarY_->getMoveButton());
    assert(scrollBarButtonX);
    assert(scrollBarButtonY);

    if (!canScrollAction_->canScroll(renderWindow, event))
        return false;
    
    static const double scrollSpeed = -1;

    vec2d deltaMove;
    if (event.mouseWheel.wheel == Mouse::Wheel::Vertical)
        deltaMove = {0, event.mouseWheel.delta * scrollSpeed};
    else
        deltaMove = {event.mouseWheel.delta * scrollSpeed, 0};

    scrollBarButtonX->move(deltaMove);
    scrollBarButtonY->move(deltaMove);
    
    return true;
}

void ScrollBarsXYManager::draw(IRenderWindow* renderWindow)
{
    if (scrollBarX_) scrollBarX_->draw(renderWindow);
    if (scrollBarY_) scrollBarY_->draw(renderWindow);
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

    if (scrollBarX_ && scrollBarX_->getWindowById(id))
        return scrollBarX_->getWindowById(id);
    
    if (scrollBarY_ && scrollBarY_->getWindowById(id))
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

void ScrollBarsXYManager::setPos (const vec2i& pos ) { pos_  = pos;  }
void ScrollBarsXYManager::setSize(const vec2u& size) { size_ = size; }

} // namespace ps
