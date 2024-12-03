#ifndef API_BAR_HPP
#define API_BAR_HPP

#include "api_photoshop.hpp"

namespace psapi
{

const wid_t kToolBarWindowId    = 101;
const wid_t kOptionsBarWindowId = 102;
const wid_t kInstrumentsBarId  = 103;

// OptionBarConstants
const wid_t kOptionFileId   = 200;
const wid_t kOptionFilterId = 201;
const wid_t kHelpOptionId   = 202;
const wid_t kOptionLayerId  = 203;
const wid_t kOptionToolsId  = 204;


const wid_t kColorPaletteId = 300;
const wid_t kOpacityBarId   = 301;
const wid_t kThicknessBarId = 302;


class IBar;

class IBarButton : public IWindow
{
public:
     enum class State
    {
        Normal,
        Hover,
        Press,
        Released,
    };

    virtual ~IBarButton() = default;

    virtual void setState(State state) = 0;
    virtual State getState() const     = 0;
};


class IOptionButton : public IBarButton
{
public:
    /**
     * @brief Calls addWindow for its bar
     */
    virtual void addOption(std::unique_ptr<IWindow> option) = 0;

    virtual void activateBar() = 0;
    virtual void deactivateBar() = 0;

    virtual IBar *getBar() = 0;
    virtual const IBar *getBar() const = 0;
};


class IBar : public IWindowContainer
{
public:
    virtual void finishButtonDraw(IRenderWindow* renderWindow, const IBarButton* button) const = 0;

    /**
     * @brief Set state Normal for all buttons in Bar
     */
    virtual bool unPressAllButtons() = 0;
};


class IColorPalette : public IWindow
{
public:
    virtual sfm::Color getColor() const = 0;

    static std::unique_ptr<IColorPalette> create();
};


class IOpacityBar : public IWindow
{
public:
    virtual float getOpacity() const = 0;

    static std::unique_ptr<IOpacityBar> create();
};


class IThicknessBar : public IWindow
{
public:
    virtual float getThickness() const = 0;

    static std::unique_ptr<IThicknessBar> create();
};

/**
 * @brief Function add window sets position
 */
class IInstrumentsBar : public IWindowContainer
{
public:
    /**
     * @brief Sets state not active for color, opacity and thickness buttons and deletes all other buttons
     */
    virtual void removeAllInstruments() = 0;
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
}

in parent:
void ConcreteBar::draw(IRenderWindow* renderWindow) {
    for (auto& button : buttons) {
        button->draw(renderWindow);
        finishButtonDraw(renderWindow, button);
    }
}

Idea: buttons in one bar are usually supposed to be pretty the same (e.g on hover they become blue,
    on release - gray. In order to make these buttons be similar, toolbar can finish their drawing based on state)

*/

} // namespace psapi

#endif // API_BAR_HPP
