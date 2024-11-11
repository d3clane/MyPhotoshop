#ifndef PLUGINS_CANVAS_SCROLLBAR_HPP
#define PLUGINS_CANVAS_SCROLLBAR_HPP

#include "api/api_photoshop.hpp"
#include "windows/windows.hpp"
#include "interpolation/include/interpolator.hpp"

namespace ps
{

class IScrollable
{
public:
    virtual void scroll(vec2f delta) = 0;
    virtual void setScroll(vec2f scroll) = 0;

    virtual vec2f getScroll() = 0;

    virtual vec2u getVisibleSize() = 0;
    virtual vec2u getFullSize() = 0;
};

class AScrollableWindow : public IScrollable, public AWindow
{
public:
    AScrollableWindow(vec2i pos, vec2u size, wid_t id) : AWindow(pos, size, id) {}
};

class PressButton : public AWindow
{
public:
    enum class State
    {
        Normal,
        Hovered,
        Pressed,
        Count // number of elements
    };
    
    PressButton(vec2i pos, vec2u size, wid_t id);

    State getState() const;

    void setShape(std::unique_ptr<IRectangleShape> shape, State state);
    
protected:
    State state_ = State::Normal;
    
    std::unique_ptr<IRectangleShape> shapes_[static_cast<size_t>(State::Count)];
};

class AScrollBarButton : public PressButton
{
public:
    AScrollBarButton(vec2i pos, vec2u size, wid_t id);

    void setSize(vec2u size);

    void move  (vec2i offset); // TODO: transformable 
    void setPos(vec2i pos);

    void setState(State state);
    
    bool update(const IRenderWindow* renderWindow, const sfm::Event& event) override;
    void draw(IRenderWindow* renderWindow) override;

    void setScrollable(AScrollableWindow* scrollable);
    const AScrollableWindow* getScrollable() const;


protected:
    void setStateFromOutside(const IRenderWindow* renderWindow);

    virtual void updateZeroScrollPos() = 0;
    virtual void updateSize() = 0;

protected:
    AScrollableWindow* scrollable_ = nullptr;

    vec2f scroll_;
    vec2i zeroScrollPos_;

    vec2i pressPos_;

    bool needToSetState_ = false;
    State stateToSet_ = State::Normal;

    bool canScrollX_ = false;
    bool canScrollY_ = false;
};

class AScrollBar : public AWindowContainer
{
public:
    AScrollBar(vec2i pos, vec2u size, wid_t id);

    bool update(const IRenderWindow* renderWindow, const Event& event) override;
    void draw(IRenderWindow* renderWindow) override;

    IWindow* getWindowById(wid_t id) override;
    const IWindow* getWindowById(wid_t id) const override;

    void addWindow(std::unique_ptr<IWindow> window) override;
    void removeWindow(wid_t id) override;

    vec2i shrinkPosToBoundaries(vec2i pos, vec2u size) const;

    void setShape(std::unique_ptr<IRectangleShape> shape);
    void setMoveButton(std::unique_ptr<AScrollBarButton> moveButton);
    AScrollBarButton* getMoveButton();

protected:
    virtual void updatePos () = 0;
    virtual void updateSize() = 0;

    void setPos (vec2i pos );
    void setSize(vec2u size);

protected:
    std::unique_ptr<IRectangleShape> shape_;

    std::unique_ptr<AScrollBarButton> moveButton_;
};

class ScrollBarX : public AScrollBar
{
public:
    ScrollBarX(vec2i pos, vec2u size, wid_t id);
protected:
    void updatePos () override;
    void updateSize() override;
};

class ScrollBarY : public AScrollBar
{
public:
    ScrollBarY(vec2i pos, vec2u size, wid_t id);
protected:
    void updatePos () override;
    void updateSize() override;
};

class ScrollBarButtonX : public AScrollBarButton
{
public:
    ScrollBarButtonX(vec2i pos, vec2u size, wid_t id);
protected:
    void updateZeroScrollPos() override;
    void updateSize()          override;
};

class ScrollBarButtonY : public AScrollBarButton
{
public:
    ScrollBarButtonY(vec2i pos, vec2u size, wid_t id);

protected:
    void updateZeroScrollPos() override;
    void updateSize()          override;
};

class ScrollBarsXYManager : public AWindowContainer
{
public:
    ScrollBarsXYManager(std::unique_ptr<ScrollBarX> scrollBarX, std::unique_ptr<ScrollBarY> scrollBarY);

    bool update(const IRenderWindow* renderWindow, const Event& event) override;
    void draw(IRenderWindow* renderWindow) override;

    void addWindow(std::unique_ptr<IWindow> window) override;
    void removeWindow(wid_t id) override;

    IWindow* getWindowById(wid_t id) override;
    const IWindow* getWindowById(wid_t id) const override;

private:
    void updatePromisedScroll (const Event& event);
    void proceedPromisedScroll(ScrollBarButtonX* scrollBarButtonX, ScrollBarButtonY* scrollBarButtonY);

private:
    std::unique_ptr<ScrollBarX> scrollBarX_;
    std::unique_ptr<ScrollBarY> scrollBarY_;

    Interpolator interpolatorX_;
    Interpolator interpolatorY_;
};
} // namespace ps

#endif // PLUGINS_CANVAS_SCROLLBAR_HPP
