#ifndef API_BAR_HPP
#define API_BAR_HPP

#include "api_photoshop.hpp"

namespace psapi {

const wid_t kToolBarWindowId    = 101;
const wid_t kOptionsBarWindowId = 102;

class IBarButton : public IWindow {
public:
    enum class State {
        Normal,
        Hover,
        Press,
        Released,
    };

    virtual void setState(State state) = 0;
    virtual State getState() const = 0; // return state
};

struct ChildInfo
{
    vec2i pos;
    vec2i size;
};

class IBar : public IWindowContainer {
public:
    virtual ChildInfo getNextChildInfo() const = 0;

    virtual void finishButtonDraw(IRenderWindow* renderWindow, const IBarButton* button) const = 0;
};

/*
finishButtonDraw basic implementation is supposed to be something like this:

void IBar::finishButtonDraw(IRenderWindow* renderWindow, const IBarButton* aButton) {
    switch (button->state()) {
        case IBarButton::State::Normal:
            break;
        case IBarButton::State::Hover:
            renderWindow->draw(hoverSprite);
            break;
        case IBarButton::State::Press:
            renderWindow->draw(pressSprite);
            break;
        case IBarButton::State::Released
            renderWindow->draw(onSprite);
            break;
    }
}

usage:
void ConcreteButton::draw(IRenderWindow* renderWindow) {
    ... // draw logic

    parent->finishButtonDraw(renderWindow, this);
}

Idea: buttons in one bar are usually supposed to be pretty the same (e.g on hover they become blue,
    on release - gray. In order to make these buttons be similar, toolbar can finish their drawing based on state)

*/

} // namespace psapi

#endif // API_BAR_HPP