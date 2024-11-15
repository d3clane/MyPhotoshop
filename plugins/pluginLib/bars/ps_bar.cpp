#include "bars/ps_bar.hpp"

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

void ABarButton::setParent(const IWindow* parent) 
{
    const ABar* parentBar = static_cast<const ABar*>(parent);

    assert(parentBar);
    parent_ = parentBar;
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

void ABarButton::setPos (vec2i pos)
{
    pos_ = pos;
}

void ABarButton::setSize(vec2u size)
{
    size_ = size;
}

// ASpritedBarButton

void ASpritedBarButton::draw(IRenderWindow* renderWindow) 
{
    renderWindow->draw(mainSprite_.get());

    assert(parent_);
    static_cast<const IBar*>(parent_)->finishButtonDraw(renderWindow, this);
}

void ASpritedBarButton::setPos(vec2i pos)
{
    pos_ = pos;
    mainSprite_->setPosition(pos.x, pos.y);
}

void ASpritedBarButton::setSize(vec2u size)
{
    size_ = size;
    mainSprite_->setScale(1.f, 1.f);
    auto spriteSize = mainSprite_->getSize();

    mainSprite_->setScale(static_cast<double>(size.x) / spriteSize.x, 
                          static_cast<double>(size.y) / spriteSize.y);
}

// ABar implementation

ABar::~ABar() = default;

void ABar::draw(IRenderWindow* renderWindow)
{
    if (!isActive_)
    {
        //std::cerr << "NOT ACTIVE\n";
        return;
    }

    //std::cerr << "ACTIVE\n";

    if (shape_)
        shape_->draw(renderWindow);

    drawChildren(renderWindow);
}

void ABar::setPos (vec2i pos)
{
    pos_ = pos;

    shape_->setPosition(pos);
}

void ABar::setSize(vec2u size)
{
    size_ = size;

    shape_->setSize(size);
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

// AShapedButtonsBar implementation

void AShapedButtonsBar::finishButtonDraw(IRenderWindow* renderWindow, const IBarButton* button) const
{
    commonOutlineShape_->setPosition(vec2i{button->getPos().x, button->getPos().y});
    for (size_t i = 0; i < static_cast<size_t>(SpriteType::Count); ++i)
        shapes_[i]->setPosition(vec2i{button->getPos().x, button->getPos().y});

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