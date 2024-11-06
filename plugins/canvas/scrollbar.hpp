#ifndef PLUGINS_CANVAS_SCROLLBAR_HPP
#define PLUGINS_CANVAS_SCROLLBAR_HPP

#include "windows/windows.hpp"

namespace ps
{
    
class ScrollBar : public AWindowVector
{
public:
    ScrollBar(vec2i pos, vec2u size, wid_t id);

    bool update(const IRenderWindow* renderWindow, const Event& event) override;
    void draw(IRenderWindow* renderWindow) override;

protected:
    std::unique_ptr<IRectangleShape> shape_;
};

class ScrollBarButton : public AWindow
{
public:
    enum class State
    {
        Normal,
        Hovered,
        Pressed,
        Count // number of elements
    };

    ScrollBarButton(vec2i pos, vec2u size, wid_t id);

    void setSize(vec2u size);
    void move(vec2i offset);

    State getState() const;

    void setShape(std::unique_ptr<IRectangleShape> shape, State state);

    bool update(const IRenderWindow* renderWindow, const sfm::Event& event) override;
    void draw(IRenderWindow* renderWindow) override;

protected:
    State state_ = State::Normal;

    std::unique_ptr<IRectangleShape> shapes_[static_cast<size_t>(State::Count)];
};

} // namespace ps

#endif // PLUGINS_CANVAS_SCROLLBAR_HPP
