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

} // namespace anonymous

Toolbar::Toolbar(vec2i pos, vec2u size) 
    : ASpritedButtonsBar(pos, size, kToolBarWindowId, getCommonBarColor(),
                         getCommonHoverTexturePath(), getCommonPressTexturePath(), 
                         getCommonReleaseTexturePath())
{
    delimeterSprite_ = createSprite(size, "media/textures/toolbarDelimeter.png");
}

void Toolbar::setChildrenInfo()
{
    static const int gapSize = 6;
    vec2u childSize = vec2u{64, 64};
    
    vec2u delimeterSpriteSize = vec2u{64, 16};

    int xMiddle = calculateMiddleForChild(childSize).x;
    delimeterSprite_.sprite->setPosition(static_cast<float>(pos_.x + xMiddle), 
                                         static_cast<float>(pos_.y));

    delimeterSprite_.sprite->setScale(1.f, 1.f);
    delimeterSprite_.sprite->setScale(
        static_cast<float>(delimeterSpriteSize.x) / static_cast<float>(delimeterSprite_.sprite->getSize().x), 
        static_cast<float>(delimeterSpriteSize.y) / static_cast<float>(delimeterSprite_.sprite->getSize().y));

    vec2i childPos;
    childPos.y = pos_.y + static_cast<int>(delimeterSprite_.sprite->getSize().y) + gapSize;
    childPos.x = xMiddle;

    for (auto& window : buttons_)
    {
        window->setPos(childPos);
        window->setSize(childSize);

        childPos.y += static_cast<int>(childSize.y + gapSize);        
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
    delimeterSprite_.sprite->draw(renderWindow);

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
