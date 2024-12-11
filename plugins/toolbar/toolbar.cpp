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
    vec2i pos  = {0, 0};
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
    int shift = static_cast<int>(static_cast<unsigned>(childIndex) * (childSize.y + gap) + gap);
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
    size_t childIndex = 0;

    for (auto& window : buttons_)
    {
        window->setPos(calculateChildPosition(childIndex, childSize_, gapSize_, 
                                              pos_, calculateMiddleForChild(childSize_)));

        window->setSize(childSize_);
        ++childIndex;
    }
}

IWindow* Toolbar::getWindowById(wid_t id)
{
    for (auto& window : buttons_)
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
    for (auto& button : buttons_)
    {
        button->draw(renderWindow);
        finishButtonDraw(renderWindow, button.get());
    }
}

void Toolbar::addWindow(std::unique_ptr<IWindow> window)
{
    IBarButton* button = nullptr;

    button = static_cast<IBarButton*>(window.get());
    assert(button);
    button->setParent(this);

    buttons_.push_back(std::unique_ptr<IBarButton>(button));
    window.release();
}

void Toolbar::removeWindow(wid_t id)
{
    for (auto it = buttons_.begin(); it != buttons_.end(); it++) 
    {
        if ((*it)->getId() == id) 
        {
            buttons_.erase(it);
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
    // TODO: could be a problem. SetSize doesn't do anything with children, using setSize from the outside
    // may ruin everything. Maybe not a problem because update is called every time.

    setPos(toolbarRect.pos);
    setChildrenInfo();

    return getActionController()->execute(
        bar_children_handler_funcs::createUpdateChildrenAction(renderWindow, event, buttons_)
    );
}

bool Toolbar::unPressAllButtons()
{
    return bar_children_handler_funcs::unPressAllButtons(buttons_);
}
