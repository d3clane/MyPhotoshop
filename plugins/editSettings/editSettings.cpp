#include "editSettings.hpp"

#include "pluginLib/bars/ps_bar.hpp"
#include "pluginLib/bars/menu.hpp"

#include <cassert>

using namespace ps;
using namespace psapi;

namespace 
{

class IClonableAction : public IAction
{
public:
    virtual std::unique_ptr<IAction> clone() const = 0;
};

class RedoLastAction : public IClonableAction
{
public:
    bool isUndoable(const Key& /* key */) override { return false; }
    bool execute(const Key& /* key */) override { return getActionController()->redo(); }
    std::unique_ptr<IAction> clone() const override { return std::make_unique<RedoLastAction>(); }
};

class UndoLastAction : public IClonableAction
{
public:
    bool isUndoable(const Key& /* key */) override { return false; };
    bool execute(const Key& /* key */) override { return getActionController()->undo(); }
    std::unique_ptr<IAction> clone() const override { return std::make_unique<UndoLastAction>(); }
};

class Button : public ANamedBarButton
{
public:
    Button(std::unique_ptr<IText> name, std::unique_ptr<IFont> font, 
           std::unique_ptr<IClonableAction> action)
    {
        name_ = std::move(name);
        font_ = std::move(font);
        action_ = std::move(action);
    }

    std::unique_ptr<IAction> createAction(const IRenderWindow* renderWindow, 
                                          const Event& event) override
    {
        updateState(renderWindow, event);

        if (state_ != State::Released)
            return nullptr;

        state_ = State::Normal;

        return action_->clone();
    }

private:
    std::unique_ptr<IClonableAction> action_;
};

std::unique_ptr<IBarButton> createCommonButton(const char* name, std::unique_ptr<IClonableAction> action)
{
    std::unique_ptr<IText> text = IText::create();
    std::unique_ptr<IFont> font = IFont::create();
    font->loadFromFile("assets/fonts/arial.ttf");
    text->setFont(font.get());
    text->setString(name);

    return std::make_unique<Button>(std::move(text), std::move(font), std::move(action));
}


} // namespace anonymous

bool onLoadPlugin()
{
    IRootWindow* rootWindow = getRootWindow();
    assert(rootWindow);

    IMenuButton* tools = static_cast<IMenuButton*>(rootWindow->getWindowById(kMenuToolsId));


    tools->addMenuItem(createCommonButton("Undo", std::make_unique<UndoLastAction>()));
    tools->addMenuItem(createCommonButton("Redo", std::make_unique<RedoLastAction>()));

    return true;
}

void onUnloadPlugin()
{
    return;
}