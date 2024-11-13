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
                                                                    Color{120, 120, 120, shapesCommonAlpha}, 
                                                                    Color{100, 100, 100, 255});
    shapes_[static_cast<size_t>(SpriteType::Press  )] = createShape(childSize_,
                                                                    Color{70, 70, 70, shapesCommonAlpha}, 
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

IWindow* Toolbar::getWindowById(wid_t id)
{
    for (auto& window : windows_)
        if (window->getId() == id)
            return window.get();
        
    if (id == id_)
        return this;
    
    return nullptr;
}

const IWindow* Toolbar::getWindowById(wid_t id) const
{
    return const_cast<Toolbar*>(this)->getWindowById(id);
}

void Toolbar::drawChildren(IRenderWindow* renderWindow)
{
    for (auto& window : windows_)
        window->draw(renderWindow);
}

void Toolbar::addWindow(std::unique_ptr<IWindow> window)
{
    ASpritedBarButton* button = nullptr;
    try
    {
        button = dynamic_cast<ASpritedBarButton*>(window.get());
        assert(button);
        button->setParent(this);
    }
    catch(...)
    {
        std::cerr << "Failed to cast window to button\n";
        assert(false);
        std::terminate();
    }

    windows_.push_back(std::unique_ptr<ASpritedBarButton>(button));
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
    vec2u renderWindowSize = renderWindow->getSize();
    setSize({renderWindowSize.x * ToolbarSize.x,
             renderWindowSize.y * ToolbarSize.y});

    setPos ({ToolbarTopLeftPos.x * renderWindowSize.x, ToolbarTopLeftPos.y * renderWindowSize.y});

    bool updatedSomeone = bar_children_handler_funcs::updateChildren(renderWindow, event, windows_);

    return updatedSomeone;
}
