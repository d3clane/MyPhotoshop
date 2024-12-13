#include "menu.hpp"

#include "pluginLib/actions/actions.hpp"

#include <cassert>

namespace ps
{

namespace 
{

// TODO: copypaste, create another file for this function
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

} // namespace anonymous

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

SubMenuBar::SubMenuBar()
{
    // TODO: move this data to the constructor params
    // create something like "create common sub menu bar" 
    // to create bar with concrete colors

    id_ = kInvalidWindowId;
    
    pos_ = {0, 0};
    size_ = {childSize_.x, 0};

    shape_ = createShape(size_, Color{120, 120, 120, 255}, Color{}, 0);
    shape_->setPosition(pos_);

    static const uint8_t shapesCommonAlpha = 100;

    commonOutlineShape_ = createShape(childSize_, Color{0, 0, 0, 0}, Color{51, 51, 51, 255});

    shapes_[static_cast<size_t>(SpriteType::Hover  )] = createShape(childSize_,
                                                                    Color{30, 144, 255, shapesCommonAlpha}, 
                                                                    Color{}, 0);

    shapes_[static_cast<size_t>(SpriteType::Press  )] = createShape(childSize_,
                                                                    Color{65, 105, 255, shapesCommonAlpha}, 
                                                                    Color{}, 0);

    // TODO: Don't need release in this kind of menu's. Simply have to do some action on release and undo

    shapes_[static_cast<size_t>(SpriteType::Release)] = createShape(childSize_,
                                                                    Color{0, 0, 139, shapesCommonAlpha},
                                                                    Color{}, 0);
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
    AShapedButtonsBar::setSize(size);
    setChildrenInfo();
}

void SubMenuBar::setPos(const vec2i& pos)
{
    AShapedButtonsBar::setPos(pos);
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
    static const int gap = 8;
    switch (spawnPosition)
    {
        case MenuButton::SubMenuSpawningDirection::Down:
            return {pos.x, pos.y + static_cast<int>(size.y) + gap};
        case MenuButton::SubMenuSpawningDirection::Right:
            return {pos.x + static_cast<int>(size.x) + gap, pos.y};
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