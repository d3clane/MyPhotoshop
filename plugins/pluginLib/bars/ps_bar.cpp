#include "bars/ps_bar.hpp"

#include <cassert>
#include <iostream>

namespace ps
{

IWindow* ABarButton::getWindowById(wid_t id) 
{
    return AWindow::getWindowById(id);
}

const IWindow* ABarButton::getWindowById(wid_t id) const 
{
    return AWindow::getWindowById(id);
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
    try
    {
        const ABar* parentBar = dynamic_cast<const ABar*>(parent);

        assert(parentBar);
        parent_ = parentBar;
    }
    catch(...)
    {
        std::cerr << "Can't cast parent to ABar\n";
        assert(false);
        std::terminate();
    }
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
    bool hovered = checkIsHovered(mousePos);
    bool pressed = updateIsPressed(event, state_ == State::Press, mousePos);
    bool clicked = checkIsClicked(event, mousePos);

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
    static_cast<const ABar*>(parent_)->finishButtonDraw(renderWindow, this);
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

void ABar::drawChildren(IRenderWindow* renderWindow) 
{
    for (const auto& window : windows_)
    {
        assert(window.get());
        window->draw(renderWindow);
    }
}

bool ABar::updateChildren(const IRenderWindow* renderWindow, const sfm::Event& event) 
{
    bool updatedSomeone = false;

    size_t lastReleasedButtonPos = static_cast<size_t>(-1); 
    size_t windowsSize = windows_.size();
    for (size_t i = 0; i < windowsSize; ++i)
    {
        ABarButton::State state = windows_[i]->getState();
        updatedSomeone |= windows_[i]->update(renderWindow, event);

        if (windows_[i]->getState() == ABarButton::State::Released && state != ABarButton::State::Released)
            lastReleasedButtonPos = i;
    }

    if (lastReleasedButtonPos != static_cast<size_t>(-1))
    {
        for (size_t i = 0; i < windowsSize; ++i)
        {
            if (i == lastReleasedButtonPos)
                continue;
            windows_[i]->setState(ABarButton::State::Normal);
        }
    }

    return updatedSomeone;
}

const IWindow* ABar::getWindowById(wid_t id) const 
{
    if (id_ == id)
        return this;
        
    for (auto& window : windows_) 
    {
        const IWindow* result = window->getWindowById(id);
        if (result)
            return result;
    }

    return nullptr;
}

ABar::~ABar() = default;

void ABar::draw(IRenderWindow* renderWindow)
{
    renderWindow->draw(shape_.get());
    
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

IWindow* ABar::getWindowById(wid_t id) 
{
    return const_cast<IWindow*>(static_cast<const ABar*>(this)->getWindowById(id));
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

} // namespace ps