#ifndef PLUGINS_CANVAS_SCROLLBAR_HPP
#define PLUGINS_CANVAS_SCROLLBAR_HPP

#include "api/api_photoshop.hpp"
#include "windows/windows.hpp"

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
    void setState(State state);

    void setShape(std::unique_ptr<IRectangleShape> shape, State state);
    
protected:
    State state_ = State::Normal;
    
    std::unique_ptr<IRectangleShape> shapes_[static_cast<size_t>(State::Count)];
};


class ScrollBarButton : public PressButton
{
public:
    ScrollBarButton(vec2i pos, vec2u size, wid_t id);

    void setSize(vec2u size);

    void move  (vec2i offset); // TODO: transformable 
    void setPos(vec2i pos);
    
    bool update(const IRenderWindow* renderWindow, const sfm::Event& event) override;
    void draw(IRenderWindow* renderWindow) override;

    void setScrollable(IScrollable* scrollable);

protected:
    void updatePos();
    void updateSize();

protected:
    IScrollable* scrollable_ = nullptr;

    float scroll_;
};

class ArrowButton : public PressButton
{
public:
    // TODO: implement
};

class ScrollBar : public AWindowContainer
{
public:
    ScrollBar(vec2i pos, vec2u size, wid_t id);

    bool update(const IRenderWindow* renderWindow, const Event& event) override;
    void draw(IRenderWindow* renderWindow) override;

    IWindow* getWindowById(wid_t id) override;
    const IWindow* getWindowById(wid_t id) const override;

    void addWindow(std::unique_ptr<IWindow> window) override;
    void removeWindow(wid_t id) override;

    vec2i shrinkPosToBoundaries(vec2i pos, vec2u size) const;

    void setShape(std::unique_ptr<IRectangleShape> shape);
    void setMoveButton(std::unique_ptr<ScrollBarButton> moveButton);

protected:
    void updatePos();
    void updateSize();

    void setPos (vec2i pos );
    void setSize(vec2u size);

protected:
    std::unique_ptr<IRectangleShape> shape_;

    std::unique_ptr<ScrollBarButton> moveButton_;
};

} // namespace ps

#endif // PLUGINS_CANVAS_SCROLLBAR_HPP
