#include "menu.hpp"

#include "pluginLib/actions/actions.hpp"
#include "interfaceInfo/interfaceInfo.hpp"

#include <cassert>

namespace ps
{

IWindow* SubMenuBar::getWindowById(wid_t id)
{
    if (id_ == id)
        return this;

    for (auto& window : buttons_)
    {
        IWindow* searchResult = window->getWindowById(id);

        if (searchResult)
            return searchResult;
    }
        
    return nullptr;
}

const IWindow* SubMenuBar::getWindowById(wid_t id) const
{
    return const_cast<SubMenuBar*>(this)->getWindowById(id);
}

static const vec2u SubMenuChildSize = {256, 32};

SubMenuBar::SubMenuBar() 
    : ASpritedButtonsBar(vec2i{0, 0}, vec2u{SubMenuChildSize.x, 0}, kInvalidWindowId, Color{60, 60, 60, 255},
                         "assets/textures/SubMenuButtonOnHover.png", 
                         "assets/textures/SubMenuButtonOnPress.png", 
                         "assets/textures/SubMenuButtonOnRelease.png")
{
    childSize_ = SubMenuChildSize;
}

void SubMenuBar::addWindow(std::unique_ptr<IWindow> window)
{
    IBarButton* button = dynamic_cast<IBarButton*>(window.get());
    assert(button);

    buttons_.push_back(std::unique_ptr<IBarButton>(button));
    window.release();

    size_.y += (childSize_.y + gapSize_);
    setSize(size_);
    
    setChildrenInfo(); // TODO: slow, can change only last added son
}

void SubMenuBar::removeWindow(wid_t id)
{
    for (auto it = buttons_.begin(); it != buttons_.end(); it++)
    {
        if ((*it)->getId() == id)
        {
            buttons_.erase(it);
            break;
        }
    }

    size_.y -= (childSize_.y + gapSize_);
    setSize(size_);
}

std::unique_ptr<IAction> SubMenuBar::createAction(const IRenderWindow* renderWindow,
                                                  const sfm::Event& event)
{
    if (!isActive_)
        return nullptr;

    return bar_children_handler_funcs::createUpdateChildrenAction(renderWindow, event, buttons_);
}

bool SubMenuBar::unPressAllButtons()
{
    return bar_children_handler_funcs::unPressAllButtons(buttons_);
}

void SubMenuBar::setChildrenInfo()
{
    size_t childIndex = 0;

    for (auto& button : buttons_)
    {
        button->setPos(vec2i{pos_.x, 
                             pos_.y + static_cast<int>(gapSize_ + (gapSize_ + childSize_.y) * childIndex)});

        button->setSize(childSize_);
        ++childIndex;
    }
}

void SubMenuBar::setSize(const vec2u& size)
{
    ASpritedButtonsBar::setSize(size);
    setChildrenInfo();
}

void SubMenuBar::setPos(const vec2i& pos)
{
    ASpritedButtonsBar::setPos(pos);
    setChildrenInfo();
}

void SubMenuBar::drawChildren(IRenderWindow* renderWindow)
{
    for (auto& button : buttons_)
    {
        button->draw(renderWindow);

        finishButtonDraw(renderWindow, button.get());
    }
}

// Menu button implementation

namespace
{

vec2i calculateSubMenuPos(const vec2i& pos, const vec2u& size, 
                          MenuButton::SubMenuSpawningDirection spawnPosition)
{
    static const int gapDown = 8;
    static const int gapRight = 0;
    switch (spawnPosition)
    {
        case MenuButton::SubMenuSpawningDirection::Down:
            return {pos.x, pos.y + static_cast<int>(size.y) + gapDown};
        case MenuButton::SubMenuSpawningDirection::Right:
            return {pos.x + static_cast<int>(size.x) + gapRight, pos.y};
        default:
            assert(false);
            return {0, 0};
    }
    
    assert(false);
    return {0, 0};
}


} // namespace anonymous

MenuButton::MenuButton(wid_t id, 
                       std::unique_ptr<IText> name, std::unique_ptr<IFont> font,
                       std::unique_ptr<SubMenuBar> subMenu,
                       SubMenuSpawningDirection spawnDirection)
    :  menu_(std::move(subMenu)), spawnDirection_(spawnDirection)
{
    id_ = id;
    font_ = std::move(font);
    name_ = std::move(name);
}

void MenuButton::draw(IRenderWindow* renderWindow)
{
    AMenuBarButton::draw(renderWindow);
    
    if (menu_)
        menu_->draw(renderWindow);
}

std::unique_ptr<IAction> MenuButton::createAction(const IRenderWindow* renderWindow,
                                                  const sfm::Event& event)
{
    return std::make_unique<UpdateCallbackAction<MenuButton>>(*this, renderWindow, event);
}

bool MenuButton::update(const IRenderWindow* renderWindow, const Event& event)
{
    bool updatedState = updateState(renderWindow, event);

    if (state_ == State::Released)
        menu_->forceActivate();
    else
        menu_->forceDeactivate(); 

    AActionController* actionController = getActionController();
    assert(actionController);

    updatedState |= actionController->execute(menu_->createAction(renderWindow, event));

    return updatedState;
}

IWindow* MenuButton::getWindowById(wid_t id)
{
    if (id_ == id)
        return this;

    return menu_->getWindowById(id);
}

const IWindow* MenuButton::getWindowById(wid_t id) const
{
    return const_cast<MenuButton*>(this)->getWindowById(id);
}

void MenuButton::addMenuItem(std::unique_ptr<IWindow> item)
{
    menu_->addWindow(std::move(item));
}

void MenuButton::activateMenu()
{
    menu_->forceActivate();
}

void MenuButton::deactivateMenu()
{
    menu_->forceDeactivate();
}

IBar* MenuButton::getMenu()
{
    return menu_.get();
}

const IBar* MenuButton::getMenu() const
{
    return const_cast<MenuButton*>(this)->getMenu();
}

void MenuButton::setPos(const vec2i& pos)
{
    AMenuBarButton::setPos(pos);
    menu_->setPos(calculateSubMenuPos(pos, size_, spawnDirection_));
}

void MenuButton::setSize(const vec2u& size)
{
    AMenuBarButton::setSize(size);
}

} // namespace ps