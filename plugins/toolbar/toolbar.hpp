#ifndef PS_TOOLBAR_TOOLBAR_HPP
#define PS_TOOLBAR_TOOLBAR_HPP

#include "pluginLib/bars/ps_bar.hpp"
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

class Toolbar : public AShapedButtonsBar 
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

    void setParent(const IWindow* parent) override;
    bool unPressAllButtons() override;

protected:
    void drawChildren(IRenderWindow* renderWindow) override;

private:
    void setChildrenInfo();

private:
    const IWindow* parent_;

    size_t gapSize_ = 16;

    size_t nextChildIndex_ = 0;

    vec2u childSize_ = {64, 64};

    std::vector<std::unique_ptr<ASpritedBarButton>> windows_;
};

} // namespace

#endif // PS_TOOLBAR_TOOLBAR_HPP
