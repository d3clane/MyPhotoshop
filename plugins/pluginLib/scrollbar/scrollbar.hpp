#ifndef PLUGINS_CANVAS_SCROLLBAR_HPP
#define PLUGINS_CANVAS_SCROLLBAR_HPP

#include "api/api_photoshop.hpp"
#include "pluginLib/windows/windows.hpp"
#include "pluginLib/interpolation/include/interpolator.hpp"

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

    virtual ~IScrollable() = default;
};

class IScrollableWindow : public IScrollable, public IWindow
{
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


    void setSize(const vec2u& size) override;
    void move  (vec2d offset); // TODO: transformable 
    void setPos(const vec2i& pos) override;

    void setState(State state);

    std::unique_ptr<IAction> createAction(const IRenderWindow* renderWindow, 
                                          const Event& event) override;
                                          
    bool update(const IRenderWindow* renderWindow, const Event& event);

    void draw(IRenderWindow* renderWindow) override;

    void setScrollable(IScrollableWindow* scrollable);
    const IScrollableWindow* getScrollable() const;

protected:
    void setStateFromOutside(const IRenderWindow* renderWindow);

    virtual void updateZeroScrollPos() = 0;
    virtual void updateSize() = 0;

protected:
    IScrollableWindow* scrollable_ = nullptr;

    vec2f scroll_;
    vec2i zeroScrollPos_;

    vec2i pressPos_;

    bool needToSetState_ = false;
    State stateToSet_ = State::Normal;

    bool canScrollX_ = false;
    bool canScrollY_ = false;
};

class AScrollBar : public IWindowContainer
{
public:
    AScrollBar(vec2i pos, vec2u size, wid_t id);

    std::unique_ptr<IAction> createAction(const IRenderWindow* renderWindow, 
                                          const Event& event) override;

    bool update(const IRenderWindow* renderWindow, const Event& event);

    void draw(IRenderWindow* renderWindow) override;

    IWindow* getWindowById(wid_t id) override;
    const IWindow* getWindowById(wid_t id) const override;

    bool isWindowContainer() const override;

    wid_t getId() const override;

    vec2i getPos() const override;
    vec2u getSize() const override;

    void setPos(const vec2i& pos) override;
    void setSize(const vec2u& size) override;

    void setParent(const IWindow* parent) override;

    void forceActivate() override;
    void forceDeactivate() override;

    bool isActive() const override;

    vec2i shrinkPosToBoundaries(vec2i pos, vec2u size) const;

    void addWindow(std::unique_ptr<IWindow> window) override;
    void removeWindow(wid_t id) override;

    void setShape(std::unique_ptr<IRectangleShape> shape);
    void setMoveButton(std::unique_ptr<AScrollBarButton> moveButton);

    AScrollBarButton* getMoveButton();

protected:
    virtual void updatePos () = 0;
    virtual void updateSize() = 0;

protected:
    wid_t id_ = kInvalidWindowId;

    vec2i pos_;
    vec2u size_;

    bool isActive_ = true;

    const IWindow* parent_ = nullptr;

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

class ScrollBarsXYManager : public IWindowContainer
{
public:
    ScrollBarsXYManager(std::unique_ptr<ScrollBarX> scrollBarX, std::unique_ptr<ScrollBarY> scrollBarY);

    std::unique_ptr<IAction> createAction(const IRenderWindow* renderWindow, 
                                          const Event& event) override;

    bool update(const IRenderWindow* renderWindow, const Event& event);

    void draw(IRenderWindow* renderWindow) override;

    IWindow* getWindowById(wid_t id) override;
    const IWindow* getWindowById(wid_t id) const override;

    bool isWindowContainer() const override;

    wid_t getId() const override;

    vec2i getPos() const override;
    vec2u getSize() const override;
    
    void setPos(const vec2i& pos) override;
    void setSize(const vec2u& size) override;

    void setParent(const IWindow* parent) override;

    void forceActivate() override;
    void forceDeactivate() override;

    bool isActive() const override;

    void addWindow(std::unique_ptr<IWindow> window) override;
    void removeWindow(wid_t id) override;

private:
    wid_t id_ = kInvalidWindowId;

    vec2i pos_;
    vec2u size_;

    bool isActive_ = true;

    const IWindow* parent_ = nullptr;

    std::unique_ptr<ScrollBarX> scrollBarX_;
    std::unique_ptr<ScrollBarY> scrollBarY_;
};
} // namespace ps

#endif // PLUGINS_CANVAS_SCROLLBAR_HPP
