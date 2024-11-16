#ifndef PS_TOOLBAR_TOOLBAR_HPP
#define PS_TOOLBAR_TOOLBAR_HPP

#include "bars/ps_bar.hpp"

extern "C" 
{
    bool   loadPlugin();
    void unloadPlugin();
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

    ChildInfo getNextChildInfo() const override;

    bool update(const IRenderWindow* renderWindow, const sfm::Event& event) override;

protected:
    void drawChildren(IRenderWindow* renderWindow) override;

private:
    size_t gapSize_ = 16;

    mutable size_t nextChildIndex_ = 0;
    mutable size_t numChildren_ = 0;

    vec2i childSize_ = {64, 64};

    std::vector<std::unique_ptr<ASpritedBarButton>> windows_;
};

} // namespace

#endif // PS_TOOLBAR_TOOLBAR_HPP
