#ifndef PS_TOOLBAR_TOOLBAR_HPP
#define PS_TOOLBAR_TOOLBAR_HPP

#include "pluginLib/bars/ps_bar.hpp"
#include "interfaceInfo/interfaceInfo.hpp"

#include <vector>

extern "C" 
{
    bool onLoadPlugin();
    void onUnloadPlugin();
}

namespace ps 
{

using namespace psapi;
using namespace psapi::sfm;

class Toolbar : public ASpritedButtonsBar 
{
public:
    Toolbar(vec2i pos, vec2u size);
    ~Toolbar() = default;

    IWindow* getWindowById(wid_t id) override;
    const IWindow* getWindowById(wid_t id) const override;

    void addWindow(std::unique_ptr<IWindow> window) override;
    void removeWindow(wid_t id) override;

    std::unique_ptr<IAction> createAction(const IRenderWindow* renderWindow,
                                          const sfm::Event& event) override;

    bool update(const IRenderWindow* renderWindow, const sfm::Event& event);

    bool unPressAllButtons() override;

protected:
    void drawChildren(IRenderWindow* renderWindow) override;

private:
    void setChildrenInfo();

private:
    std::vector<std::unique_ptr<IBarButton>> buttons_;

    SpriteInfo delimeterSprite_; 
};

} // namespace

#endif // PS_TOOLBAR_TOOLBAR_HPP
