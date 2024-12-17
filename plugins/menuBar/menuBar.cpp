#include "menuBar.hpp"
#include "interfaceInfo/interfaceInfo.hpp"

#include "pluginLib/bars/ps_bar.hpp"
#include "pluginLib/actions/actions.hpp"
#include "pluginLib/bars/menu.hpp"

#include <vector>
#include <memory>
#include <cassert>

using namespace ps;
using namespace psapi;
using namespace psapi::sfm;

class MenuBar : public ASpritedButtonsBar
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

private:
    std::vector<std::unique_ptr<IMenuButton>> buttons_;
};

namespace
{

ASpritedButtonsBar::SpriteInfo createSprite(const vec2u& size, const char* filename)
{
    ASpritedButtonsBar::SpriteInfo info;
    info.sprite = ISprite::create();
    info.texture = ITexture::create();

    info.texture->loadFromFile(filename);
    info.sprite->setTexture(info.texture.get());

    info.sprite->setScale(static_cast<float>(size.x) / static_cast<float>(info.texture->getSize().x), 
                          static_cast<float>(size.y) / static_cast<float>(info.texture->getSize().y));

    return info;
}

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

MenuBar::MenuBar()
{
    id_ = kMenuBarWindowId;

    pos_ = {0, 0};
    size_ = {1, 1};
    
    // TODO: more pretty colors

    shape_ = createShape(size_, getCommonBarColor(), Color{}, 0);
    shape_->setPosition(pos_);

    shape_->setPosition(pos_);

    sprites_[static_cast<size_t>(SpriteType::Hover  )] = createSprite(size_, getCommonHoverTexturePath());
    sprites_[static_cast<size_t>(SpriteType::Press  )] = createSprite(size_, getCommonPressTexturePath());
    sprites_[static_cast<size_t>(SpriteType::Release)] = createSprite(size_, getCommonReleaseTexturePath());
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
    static const size_t deltaSizeButton = 16;

    static const int gapSize = 1;
    vec2i childPos = pos_ + vec2i{gapSize, gapSize};

    static const size_t buttonYSize = 18 + 4 + 4;

    for (auto& buttonAbstract : buttons_)
    {
        MenuButton* button = dynamic_cast<MenuButton*>(buttonAbstract.get());
        assert(button);

        vec2u textSize_ = button->getTextSize();
        vec2u buttonSize = vec2u{ textSize_.x + deltaSizeButton, buttonYSize };
        button->setPos(childPos);
        childPos += vec2i{static_cast<int>(buttonSize.x + gapSize), 0};

        button->setSize(buttonSize);
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

    const char* menuButtonsNames[] = {"File", "Tools", "Layer", "Filters", "Help"};
    const int menuButtonsIds[] = {kMenuFileId, kMenuToolsId, kMenuLayerId, kMenuFilterId, kMenuHelpId};
    static const size_t menuButtonsCount = sizeof(menuButtonsNames) / sizeof(char*);

    for (size_t i = 0; i < menuButtonsCount; ++i)
    {
        std::unique_ptr<IText> text = IText::create();
        std::unique_ptr<IFont> font = IFont::create();
        font->loadFromFile("media/fonts/arial.ttf");

        text->setFont(font.get());
        text->setString(menuButtonsNames[i]);

        auto subMenuBar = std::make_unique<SubMenuBar>();
        auto menuButton = std::make_unique<MenuButton>(menuButtonsIds[i], 
                                                       std::move(text), std::move(font), 
                                                       std::move(subMenuBar));

        menu->addWindow(std::move(menuButton));
    }

    rootWindow->addWindow(std::move(menu));
    
    return true;
}