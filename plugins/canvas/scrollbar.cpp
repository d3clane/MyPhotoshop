#include "scrollbar.hpp"

namespace ps
{

// Scrollbar implementation

ScrollBar::ScrollBar(vec2i pos, vec2u size, wid_t id) : AWindowVector(pos, size, id)
{
}

bool ScrollBar::update(const IRenderWindow* renderWindow, const Event& event)
{
    return updateChildren(renderWindow, event);
}

void ScrollBar::draw(IRenderWindow* renderWindow)
{
    shape_->draw(renderWindow);

    drawChildren(renderWindow);
}

// Scrollbar button

ScrollBarButton::ScrollBarButton(vec2i pos, vec2u size, wid_t id) : AWindow(pos, size, id)
{
}

void ScrollBarButton::setSize(vec2u size)
{
    for (auto& shape : shapes_)
        shape->setSize(size);

    size_ = size;
}

void ScrollBarButton::move(vec2i offset)
{
    for (auto& shape : shapes_)
        shape->move(vec2f{static_cast<float>(offset.x), static_cast<float>(offset.y)});

    pos_ += offset;
}

ScrollBarButton::State ScrollBarButton::getState() const
{
    return state_;
}

void ScrollBarButton::setShape(std::unique_ptr<IRectangleShape> shape, State state)
{
    shapes_[static_cast<size_t>(state)] = std::move(shape);
}

void ScrollBarButton::draw(IRenderWindow* renderWindow)
{
    shapes_[static_cast<size_t>(state_)]->draw(renderWindow);
}

bool ScrollBarButton::update(const IRenderWindow* renderWindow, const sfm::Event& event)
{
    vec2i mousePos = sfm::Mouse::getPosition();

    bool isPressed = updateIsPressed(event, state_ == State::Pressed, mousePos);
    bool isHovered = checkIsHovered(mousePos);

    state_ = State::Normal;
    state_ = isHovered ? State::Hovered : state_;
    state_ = isPressed ? State::Pressed : state_;

    return isPressed;
}

} // namespace ps
