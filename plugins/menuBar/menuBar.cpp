#include "menuBar.hpp"

#include "pluginLib/bars/ps_bar.hpp"
#include "pluginLib/actions/actions.hpp"
#include "pluginLib/bars/menu.hpp"

#include <vector>
#include <memory>
#include <cassert>

using namespace ps;
using namespace psapi;
using namespace psapi::sfm;

class MenuBar : public AShapedButtonsBar
{
public:
    MenuBar();

    bool unPressAllButtons() override;

    std::unique_ptr<IAction> createAction(const IRenderWindow* renderWindow, const Event& event) override;
    bool update(const IRenderWindow* renderWindow, const Event& event);

    IWindow* getWindowById(wid_t id) override;
    const IWindow* getWindowById(wid_t id) const override;

    void addWindow(std::unique_ptr<IWindow> window) override;
    void removeWindow(wid_t id) override;

protected:
    void drawChildren(IRenderWindow* renderWindow) override;

    void setChildrenInfo();

    vec2u childSize_ = {64, 32};
    size_t gapSize_ = 16;

private:
    std::vector<std::unique_ptr<IMenuButton>> buttons_;
};

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
    return vec2i{ parentPos.x + shift, middle.y };
}

} // namespace anonymous

MenuBar::MenuBar()
{
    id_ = kMenuBarWindowId;

    pos_ = {0, 0};
    size_ = {1, 1};
    
    // TODO: more pretty colors

    shape_ = createShape(size_, Color{120, 120, 120, 255}, Color{}, 0);
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

bool MenuBar::unPressAllButtons()
{
    return bar_children_handler_funcs::unPressAllButtons(buttons_);
}

std::unique_ptr<IAction> MenuBar::createAction(const IRenderWindow* renderWindow, const Event& event)
{
    return std::make_unique<UpdateCallbackAction<MenuBar>>(*this, renderWindow, event);
}

void MenuBar::setChildrenInfo()
{
    size_t childIndex = 0;

    for (auto& button : buttons_)
    {
        button->setPos(calculateChildPosition(childIndex, childSize_, gapSize_, 
                                              pos_, calculateMiddleForChild(childSize_)));

        button->setSize(childSize_);
        ++childIndex;
    }
}

bool MenuBar::update(const IRenderWindow* renderWindow, const Event& event)
{
    IntRect menuBarRect = getMenuBarIntRect();
    // TODO: could be a problem. SetSize doesn't do anything with children, using setSize from the outside
    // may ruin everything. Maybe not a problem because update is called every time.
    setSize(menuBarRect.size); 
    setPos(menuBarRect.pos);
    setChildrenInfo();
    
    AActionController* actionController = getActionController();
    assert(actionController);

    // TODO: check on press outside the box of the menu probably

    return actionController->execute(
        bar_children_handler_funcs::createUpdateChildrenAction(renderWindow, event, buttons_));
}

IWindow* MenuBar::getWindowById(wid_t id)
{
    if (id == id_)
        return this;
        
    for (auto& window : buttons_)
    {
        IWindow* searchResult = window->getWindowById(id);

        if (searchResult)
            return searchResult;
    }

    return nullptr;
}

const IWindow* MenuBar::getWindowById(wid_t id) const
{
    return const_cast<MenuBar*>(this)->getWindowById(id);
}

void MenuBar::addWindow(std::unique_ptr<IWindow> window)
{
    IMenuButton* menuButton = dynamic_cast<IMenuButton*>(window.get());
    assert(menuButton);

    buttons_.push_back(std::unique_ptr<IMenuButton>(menuButton));
    window.release();
}

void MenuBar::removeWindow(wid_t id)
{
    for (auto it = buttons_.begin(); it != buttons_.end(); ++it)
    {
        if ((*it)->getId() == id)
        {
            buttons_.erase(it);
            return;
        }
    }
}

void MenuBar::drawChildren(IRenderWindow* renderWindow)
{
    for (auto& button : buttons_)
    {
        button->draw(renderWindow);
        finishButtonDraw(renderWindow, button.get());
    }
}

bool onLoadPlugin()
{
    IRootWindow* rootWindow = getRootWindow();
    auto menu = std::make_unique<MenuBar>();

    std::unique_ptr<IText> text = IText::create();
    std::unique_ptr<IFont> font = IFont::create();
    font->loadFromFile("media/fonts/arial.ttf");

    text->setFont(font.get());
    text->setString("Filters");

    auto subMenuBar = std::make_unique<SubMenuBar>();
    auto menuButton = std::make_unique<MenuButton>(kMenuFilterId, 
                                                   std::move(text), std::move(font), 
                                                   std::move(subMenuBar));

    menu->addWindow(std::move(menuButton));

    rootWindow->addWindow(std::move(menu));
    
    return true;
}