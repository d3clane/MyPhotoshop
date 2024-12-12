#include "ps_bar.hpp"

#include <cassert>
#include <iostream>

namespace ps
{

IWindow* ABarButton::getWindowById(wid_t id) 
{
    return id == id_ ? this : nullptr;
}

const IWindow* ABarButton::getWindowById(wid_t id) const 
{
    return id == id_ ? this : nullptr;
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

bool ABarButton::updateState(const IRenderWindow* renderWindow, const Event& event)
{
    vec2i mousePos = Mouse::getPosition(renderWindow);
    bool hovered = checkIsHovered(mousePos, pos_, size_);
    bool pressed = updateIsPressed(event, state_ == State::Press, hovered);
    bool clicked = checkIsClicked(event, hovered);

    if (clicked)
    {
        if (state_ != State::Released)
            state_ = State::Released;
        else
            state_ = State::Normal;
    }
    
    if (state_ != State::Released)
    {
        state_ = State::Normal;
        if (hovered) state_ = State::Hover;
        if (pressed) state_ = State::Press;
    }

    return clicked || hovered || pressed;
}

void ABarButton::setPos(const vec2i& pos)
{
    pos_ = pos;
}

void ABarButton::setSize(const vec2u& size)
{
    size_ = size;
}

void ABarButton::setParent(const IWindow* parent)
{
    parent_ = parent;
}

// ASpritedBarButton

void ASpritedBarButton::setPos(const vec2i& pos)
{
    pos_ = pos;
    mainSprite_->setPosition(pos.x, pos.y);
}

void ASpritedBarButton::setSize(const vec2u& size)
{
    size_ = size;
    mainSprite_->setScale(1.f, 1.f);
    vec2u spriteSize = mainSprite_->getSize();

    mainSprite_->setScale(static_cast<float>(size.x) / static_cast<float>(spriteSize.x), 
                          static_cast<float>(size.y) / static_cast<float>(spriteSize.y));
}

void ASpritedBarButton::draw(IRenderWindow* renderWindow)
{
    if (!isActive_)
        return;

    mainSprite_->draw(renderWindow);
}

// ANamedBarButton implementation

static const float textShiftFromOneSide = 0.1f;

// TODO: copypaste with AMenuBarButton
void ANamedBarButton::setPos (const vec2i& pos)
{
    pos_ = pos;

    name_->setPos({static_cast<float>(pos.x) + static_cast<float>(size_.x * textShiftFromOneSide), 
                   static_cast<float>(pos.y) + static_cast<float>(size_.y * textShiftFromOneSide)});
}

void ANamedBarButton::setSize(const vec2u& size)
{
    size_ = size;

    static const float textSizeRatio = 1.f - 2.f * textShiftFromOneSide;
    name_->setSize(textSizeRatio * vec2f{static_cast<float>(size.x), static_cast<float>(size.y)});

    setPos(pos_);
}

void ANamedBarButton::draw(IRenderWindow* renderWindow)
{
    name_->draw(renderWindow);
}

// AMenuBarButton

vec2i AMenuBarButton::getPos() const { return pos_; }

vec2u AMenuBarButton::getSize() const { return size_; }

void AMenuBarButton::setPos(const vec2i& pos)
{
    pos_ = pos;

    name_->setPos({static_cast<float>(pos.x) + static_cast<float>(size_.x * textShiftFromOneSide), 
                   static_cast<float>(pos.y) + static_cast<float>(size_.y * textShiftFromOneSide)});
}

void AMenuBarButton::setSize(const vec2u& size)
{
    size_ = size;

    static const float textSizeRatio = 1.f - 2.f * textShiftFromOneSide;
    name_->setSize(textSizeRatio * vec2f{static_cast<float>(size.x), static_cast<float>(size.y)});

    setPos(pos_);
}

void AMenuBarButton::setParent(const IWindow* parent) { parent_ = parent; }

wid_t AMenuBarButton::getId() const { return id_; }

void AMenuBarButton::forceDeactivate() { isActive_ = true; }
void AMenuBarButton::forceActivate() { isActive_ = false; }

bool AMenuBarButton::isActive() const { return isActive_; }
bool AMenuBarButton::isWindowContainer() const { return false; } 

void AMenuBarButton::setState(State state)
{
    state_ = state;
}

AMenuBarButton::State AMenuBarButton::getState() const { return state_; }

void AMenuBarButton::draw(IRenderWindow* renderWindow)
{
    if (!isActive_)
        return;

    name_->draw(renderWindow);
}

// TODO: copypaste
bool AMenuBarButton::updateState(const IRenderWindow* renderWindow, const Event& event)
{
    vec2i mousePos = Mouse::getPosition(renderWindow);
    bool hovered = checkIsHovered(mousePos, pos_, size_);
    bool pressed = updateIsPressed(event, state_ == State::Press, hovered);
    bool clicked = checkIsClicked(event, hovered);

    if (clicked)
    {
        if (state_ != State::Released)
            state_ = State::Released;
        else
            state_ = State::Normal;
    }
    
    if (state_ != State::Released)
    {
        state_ = State::Normal;
        if (hovered) state_ = State::Hover;
        if (pressed) state_ = State::Press;
    }

    return clicked || hovered || pressed;
}

// ABar implementation

ABar::~ABar() = default;

void ABar::draw(IRenderWindow* renderWindow)
{
    if (!isActive_)
        return;

    if (shape_)
        shape_->draw(renderWindow);

    drawChildren(renderWindow);
}

void ABar::setPos(const vec2i& pos)
{
    pos_ = pos;

    shape_->setPosition(pos);
}

void ABar::setSize(const vec2u& size)
{
    size_ = size;

    shape_->setSize(size);
}

void ABar::setParent(const IWindow* parent) 
{
    parent_ = parent;
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
    return id_;
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

vec2i ABar::calculateMiddleForChild(vec2u childSize)
{
    vec2i middle = vec2i(pos_.x + static_cast<int>(size_.x - childSize.x) / 2, 
                         pos_.y + static_cast<int>(size_.y - childSize.y) / 2);

    return middle;
}


// AShapedButtonsBar implementation

void AShapedButtonsBar::finishButtonDraw(IRenderWindow* renderWindow, const IBarButton* button) const
{
    commonOutlineShape_->setPosition(vec2i{button->getPos().x, button->getPos().y});
    for (size_t i = 0; i < static_cast<size_t>(SpriteType::Count); ++i)
    {
        shapes_[i]->setPosition(vec2i{button->getPos().x, button->getPos().y});
    }

    switch (button->getState()) 
    {
        case IBarButton::State::Normal:
            break;
        case IBarButton::State::Hover:
            renderWindow->draw(shapes_[static_cast<size_t>(SpriteType::Hover)].get());
            renderWindow->draw(commonOutlineShape_.get());
            break;
        case IBarButton::State::Press:
            renderWindow->draw(shapes_[static_cast<size_t>(SpriteType::Press)].get());
            renderWindow->draw(commonOutlineShape_.get());
            break;
        case IBarButton::State::Released:
            renderWindow->draw(shapes_[static_cast<size_t>(SpriteType::Release)].get());
            renderWindow->draw(commonOutlineShape_.get());
            break;

        default:
            assert(false);
            std::terminate();
            break;
    }
}

void AShapedButtonsBar::setShape(std::unique_ptr<IRectangleShape> shape, SpriteType pos)
{
    assert(static_cast<size_t>(pos) < static_cast<size_t>(SpriteType::Count));

    shapes_[static_cast<size_t>(pos)] = std::move(shape);
}

} // namespace ps