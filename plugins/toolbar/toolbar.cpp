#include "toolbar.hpp"

#include <cassert>
#include <iostream>
#include <memory>

using namespace ps;

extern "C" 
{
    
bool loadPlugin() 
{
    vec2i pos  = {0,   0  };
    vec2u size = {0, 0};
    auto toolbar = std::make_unique<Toolbar>(pos, size);

    getRootWindow()->addWindow(std::move(toolbar));
    
    return true;
}

void unloadPlugin() 
{
    getRootWindow()->removeWindow(kToolBarWindowId);
}

}

namespace
{

std::unique_ptr<IRectangleShape> createShape(const vec2i& size, 
                                             const Color& color = {}, const Color& outlineColor = {}, 
                                             const float outlineThickness = 1)
{
    auto shape = IRectangleShape::create(size.x, size.y);

    shape->setFillColor(color);
    shape->setOutlineThickness(outlineThickness);
    shape->setOutlineColor(outlineColor);

    return shape;
}

} // namespace anonymous

Toolbar::Toolbar(vec2i pos, vec2u size) 
{
    id_ = kToolBarWindowId;
    
    pos_ = pos;
    size_ = size;

    shape_ = createShape(vec2i{size_.x, size_.y}, Color{120, 120, 120, 255}, Color{}, 0);
    shape_->setPosition(pos_);

    static const uint8_t shapesCommonAlpha = 100;

    commonOutlineShape_ = createShape(childSize_, Color{0, 0, 0, 0}, Color{51, 51, 51, 255});

    shapes_[static_cast<size_t>(SpriteType::Hover  )] = createShape(childSize_,
                                                                    Color{70, 70, 70, shapesCommonAlpha}, 
                                                                    Color{100, 100, 100, 255});
    shapes_[static_cast<size_t>(SpriteType::Press  )] = createShape(childSize_,
                                                                    Color{128, 128, 128, shapesCommonAlpha}, 
                                                                    Color{100, 100, 100, 255});
    shapes_[static_cast<size_t>(SpriteType::Release)] = createShape(childSize_,
                                                                    Color{94 , 125, 147, shapesCommonAlpha}, 
                                                                    Color{112, 140, 160, 255});
}

ChildInfo Toolbar::getNextChildInfo() const 
{
    vec2i pos = {gapSize_, gapSize_ + (gapSize_ + childSize_.x) * nextChildIndex_};

    nextChildIndex_++;

    return {pos, childSize_};
}

void Toolbar::finishButtonDraw(IRenderWindow* renderWindow, const IBarButton* button) const
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

void Toolbar::addWindow(std::unique_ptr<IWindow> window)
{
    ABarButton* button = nullptr;
    try
    {
        button = dynamic_cast<ABarButton*>(window.get());
        assert(button);
        button->setParent(this);
    }
    catch(...)
    {
        std::cerr << "Failed to cast window to button\n";
        assert(false);
        std::terminate();
    }

    windows_.push_back(std::unique_ptr<ABarButton>(button));
    window.release();
}

void Toolbar::removeWindow(wid_t id)
{
    for (auto it = windows_.begin(); it != windows_.end(); it++) 
    {
        if ((*it)->getId() == id) 
        {
            windows_.erase(it);
            return;
        }
    }
}

bool Toolbar::update(const IRenderWindow* renderWindow, const sfm::Event& event) 
{
    auto renderWindowSize = renderWindow->getSize();
    setSize({renderWindowSize.x * ToolbarSize.x,
             renderWindowSize.y * ToolbarSize.y});

    setPos ({ToolbarTopLeftPos.x * renderWindowSize.x, ToolbarTopLeftPos.y * renderWindowSize.y});

    bool updatedSomeone = updateChildren(renderWindow, event);

    return updatedSomeone;
}
