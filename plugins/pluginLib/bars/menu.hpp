#ifndef PLUGINS_PLUGIN_LIB_BARS_MENU_HPP
#define PLUGINS_PLUGIN_LIB_BARS_MENU_HPP

#include "ps_bar.hpp"

#include <vector>
#include <memory>

namespace ps
{

class SubMenuBar : public AShapedButtonsBar
{
public:
    SubMenuBar();

    IWindow* getWindowById(wid_t id) override;
    const IWindow* getWindowById(wid_t id) const override;

    void addWindow(std::unique_ptr<IWindow> window) override;
    void removeWindow(wid_t id) override;

    std::unique_ptr<IAction> createAction(const IRenderWindow* renderWindow,
                                          const sfm::Event& event) override;

    bool unPressAllButtons() override;

    void setPos(const vec2i& pos) override;
    void setSize(const vec2u& size) override;

protected:
    void drawChildren(IRenderWindow* renderWindow) override;

private:
    void setChildrenInfo();

private:
    size_t gapSize_ = 1;
    vec2u childSize_ = {128, 16};

    std::vector<std::unique_ptr<IBarButton>> buttons_;
};

class MenuButton : public AMenuBarButton
{
public:
    enum class SubMenuSpawningDirection
    {
        Down,
        Right,
    };

public:
    MenuButton(wid_t id, 
               std::unique_ptr<IText> text, std::unique_ptr<IFont> font,
               std::unique_ptr<SubMenuBar> subMenu, 
               SubMenuSpawningDirection spawnDirection = SubMenuSpawningDirection::Down);

    void draw(IRenderWindow* renderWindow) override;

    std::unique_ptr<IAction> createAction(const IRenderWindow* renderWindow,
                                          const sfm::Event& event) override;

    bool update(const IRenderWindow* renderWindow, const Event& event);
    
    IWindow* getWindowById(wid_t id) override;
    const IWindow* getWindowById(wid_t id) const override;

    void addMenuItem(std::unique_ptr<IWindow> item) override;

    void activateMenu() override;
    void deactivateMenu() override;

    IBar *getMenu() override;
    const IBar *getMenu() const override;

    void setPos(const vec2i& pos) override;
    void setSize(const vec2u& size) override;

private:
    std::unique_ptr<SubMenuBar> menu_;
    SubMenuSpawningDirection spawnDirection_;
};

} // namespace ps

#endif // PLUGINS_PLUGIN_LIB_BARS_MENU_HPP