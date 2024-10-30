#ifndef PS_TOOLBAR_TOOLBAR_HPP
#define PS_TOOLBAR_TOOLBAR_HPP

#include "bars/ps_bar.hpp"

extern "C" {
    bool   loadPlugin();
    void unloadPlugin();
}

namespace ps {

using namespace psapi;
using namespace psapi::sfm;

class Toolbar : public ABar {
private:
    struct SpriteInfo
    {
        std::unique_ptr<ISprite> sprite;
        std::unique_ptr<ITexture> texture;
    };
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

protected:
    void finishButtonDraw(IRenderWindow* renderWindow, const IBarButton* button) const override; 
    
    void loadSprite(SpriteType type, const std::string& path);

private:
    int gapSize_ = 16;

    mutable int nextChildIndex_ = 0;
    mutable int numChildren_ = 0;

    vec2i childSize_ = {64, 64};

    SpriteInfo sprites_[static_cast<size_t>(SpriteType::Count)];
};

} // namespace

#endif // PS_TOOLBAR_TOOLBAR_HPP
