#include "toolbar.hpp"
#include "pluginLib/actions/actions.hpp"

#include <cassert>
#include <iostream>
#include <memory>

using namespace ps;

extern "C" 
{
    
bool onLoadPlugin() 
{
    vec2i pos  = {0,   0  };
    vec2u size = {0, 0};
    auto toolbar = std::make_unique<Toolbar>(pos, size);

    getRootWindow()->addWindow(std::move(toolbar));
    
    return true;
}

void onUnloadPlugin() 
{
    getRootWindow()->removeWindow(kToolBarWindowId);
}

}

namespace
{

std::unique_ptr<IRectangleShape> createShape(const vec2u& size, 
                                             const Color& color = {}, const Color& outlineColor = {}, 
                                             const float outlineThickness = 1)
{
    auto shape = IRectangleShape::create(size.x, size.y);

    shape->setFillColor(color);
    shape->setOutlineThickness(outlineThickness);
    shape->setOutlineColor(outlineColor);

    return shape;
}

vec2i calculateChildPosition(size_t childIndex, vec2u childSize, size_t gap, 
                             vec2i parentPos, vec2i middle)
{
    int shift = static_cast<int>(static_cast<unsigned>(childIndex) * (childSize.x + gap) + gap);
    return vec2i{ middle.x, parentPos.y + shift };
}

} // namespace anonymous

Toolbar::Toolbar(vec2i pos, vec2u size) 
{
    id_ = kToolBarWindowId;
    
    pos_ = pos;
    size_ = size;

    shape_ = createShape(size, Color{120, 120, 120, 255}, Color{}, 0);

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

void Toolbar::setChildrenInfo()
{
    nextChildIndex_ = 0;

    for (auto& window : windows_)
    {
        window->setPos(calculateChildPosition(nextChildIndex_, childSize_, gapSize_, 
                                              pos_, calculateMiddleForChild(childSize_)));

        window->setSize(childSize_);
        ++nextChildIndex_;
    }
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

    button = static_cast<ASpritedBarButton*>(window.get());
    assert(button);
    button->setParent(this);

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

std::unique_ptr<IAction> Toolbar::createAction(const IRenderWindow* renderWindow,
                                               const sfm::Event& event)
{
    return std::make_unique<UpdateCallbackAction<Toolbar>>(*this, renderWindow, event);
}

bool Toolbar::update(const IRenderWindow* renderWindow, const sfm::Event& event) 
{
    IntRect toolbarRect = getToolbarIntRect();
    setSize(toolbarRect.size);
    setPos(toolbarRect.pos);
    setChildrenInfo();

    return getActionController()->execute(
        bar_children_handler_funcs::createUpdateChildrenAction(renderWindow, event, windows_)
    );
}

void Toolbar::setParent(const IWindow* parent)
{
    parent_ = parent;
}


bool Toolbar::unPressAllButtons()
{
    return bar_children_handler_funcs::unPressAllButtons(windows_);
}
