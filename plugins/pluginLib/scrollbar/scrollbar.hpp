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

class ICanScrollAction
{
public:
    virtual bool canScroll(const IRenderWindow* renderWindow, const Event& event) = 0;

    virtual ~ICanScrollAction() = default;
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

    void setInsideSprite(SpriteInfo sprite, State state);
    void setLeftBoundarySprite(SpriteInfo sprite);
    void setRightBoundarySprite(SpriteInfo sprite);

protected:
    virtual void configureSprites() = 0;

    State state_ = State::Normal;
    
    SpriteInfo sprites_[static_cast<size_t>(State::Count)];
    SpriteInfo leftBoundary_;
    SpriteInfo rightBoundary_;
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

    void setScrollable(IScrollable* scrollable);
    const IScrollable* getScrollable() const;

    void setZeroSCrollPos(vec2i pos);
    
protected:
    void setStateFromOutside(const IRenderWindow* renderWindow);

    virtual void updateSize() = 0;

protected:
    IScrollable* scrollable_ = nullptr;

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
    AScrollBar(vec2i pos, vec2u size, vec2i deltaFromPos, vec2u deltaFromSize, wid_t id);

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

    virtual vec2i shrinkPosToBoundaries(vec2i pos, vec2u size) const = 0;

    void addWindow(std::unique_ptr<IWindow> window) override;
    void removeWindow(wid_t id) override;

    void setSprite(SpriteInfo sprite);
    void setMoveButton(std::unique_ptr<AScrollBarButton> moveButton);

    AScrollBarButton* getMoveButton();

protected:
    wid_t id_ = kInvalidWindowId;

    vec2i pos_;
    vec2u size_;

    bool isActive_ = true;
    const IWindow* parent_ = nullptr;

    SpriteInfo sprite_;

    std::unique_ptr<AScrollBarButton> moveButton_;

    vec2i deltaFromPos_;
    vec2u deltaFromSize_;
};

class ScrollBarX : public AScrollBar
{
public:
    ScrollBarX(vec2i pos, vec2u size, vec2i deltaFromPos, vec2u deltaFromSize, wid_t id);

    vec2i shrinkPosToBoundaries(vec2i pos, vec2u size) const override;
};

class ScrollBarY : public AScrollBar
{
public:
    ScrollBarY(vec2i pos, vec2u size, vec2i deltaFromPos, vec2u deltaFromSize, wid_t id);

    vec2i shrinkPosToBoundaries(vec2i pos, vec2u size) const override;
};

class ScrollBarButtonX : public AScrollBarButton
{
public:
    ScrollBarButtonX(vec2i pos, vec2u size, wid_t id);

protected:
    void configureSprites() override;
    void updateSize() override;
};

class ScrollBarButtonY : public AScrollBarButton
{
public:
    ScrollBarButtonY(vec2i pos, vec2u size, wid_t id);

protected:
    void configureSprites() override;
    void updateSize() override;
};

// TODO: a square between
class ScrollBarsXYManager : public IWindowContainer
{
public:
    ScrollBarsXYManager(std::unique_ptr<ScrollBarX> scrollBarX, std::unique_ptr<ScrollBarY> scrollBarY,
                        std::unique_ptr<ICanScrollAction> canScrollAction);

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
    
    std::unique_ptr<ICanScrollAction> canScrollAction_;
};

} // namespace ps

#endif // PLUGINS_CANVAS_SCROLLBAR_HPP
