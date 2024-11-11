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

class Toolbar : public ABar 
{
private:
    enum class SpriteType
    {
        Hover = 0,
        Press,
        Release,
        Count, // count of elements
    };

public:
    Toolbar(vec2i pos, vec2u size);
    ~Toolbar() = default;

    void addWindow(std::unique_ptr<IWindow> window) override;
    void removeWindow(wid_t id) override;

    ChildInfo getNextChildInfo() const override;

    bool update(const IRenderWindow* renderWindow, const sfm::Event& event) override;

protected:
    void finishButtonDraw(IRenderWindow* renderWindow, const IBarButton* button) const override; 

private:
    size_t gapSize_ = 16;

    mutable int nextChildIndex_ = 0;
    mutable int numChildren_ = 0;

    vec2i childSize_ = {64, 64};

    std::unique_ptr<IRectangleShape> commonOutlineShape_;
    std::unique_ptr<IRectangleShape> shapes_[static_cast<size_t>(SpriteType::Count)];
};

} // namespace

#endif // PS_TOOLBAR_TOOLBAR_HPP
