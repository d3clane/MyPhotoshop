#ifndef PLUGINS_PLUGIN_LIB_INSTRUMENTS_BAR_HPP
#define PLUGINS_PLUGIN_LIB_INSTRUMENTS_BAR_HPP

#include "bars/ps_bar.hpp"
#include "windows/windows.hpp"

namespace ps
{

static const wid_t kInstrumentsBarId = 10001;

class InstrumentsBar;

class ColorButton : public ABarButton
{
public:
    void draw(IRenderWindow* renderWindow) override;
    bool update(const IRenderWindow* renderWindow, const Event& event) override;

protected:
    std::unique_ptr<ABar> colorChoiceBar_;

    std::unique_ptr<IText> text_;
};

class InstrumentsBar : public ABar
{
public:
    bool update(const IRenderWindow* renderWindow, const Event& event) override;

protected:
    wid_t id_ = kInvalidWindowId;

    const IWindow* parent_ = nullptr;
    bool isActive_ = true;

    vec2i pos_;
    vec2u size_;

    std::unique_ptr<IRectangleShape> shape_;
    
    std::vector<std::unique_ptr<AWindow>> windows_;

protected:
    void   drawChildren(IRenderWindow* renderWindow);
    bool updateChildren(const IRenderWindow* renderWindow, const sfm::Event& event);

    void setPos (vec2i pos);
    void setSize(vec2u size);

private:
    enum class SpriteType
    {
        Hover = 0,
        Press,
        Release,
        Count, // count of elements
    };

public:
    InstrumentsBar(vec2i pos, vec2u size);

    void addWindow(std::unique_ptr<IWindow> window) override;
    void removeWindow(wid_t id) override;

    ChildInfo getNextChildInfo() const override;

    bool update(const IRenderWindow* renderWindow, const sfm::Event& event) override;

protected:
    void finishButtonDraw(IRenderWindow* renderWindow, const IBarButton* button) const override;

private:
    size_t gapSize_ = 16;

    std::unique_ptr<IRectangleShape> commonOutlineShape_;
    std::unique_ptr<IRectangleShape> shapes_[static_cast<size_t>(SpriteType::Count)];
}; 

} // namespace ps

#endif // PLUGINS_PLUGIN_LIB_INSTRUMENTS_BAR_HPP