#ifndef PLUGINS_PLUGIN_LIB_INSTRUMENTS_BAR_HPP
#define PLUGINS_PLUGIN_LIB_INSTRUMENTS_BAR_HPP

#include "bars/ps_bar.hpp"
#include "windows/windows.hpp"
#include "mediator.hpp"
#include "actions.hpp"

namespace ps
{

static const wid_t kInstrumentBarId = 10001;

class InstrumentBar : public AShapedButtonsBar
{
public:
    InstrumentBar();

    IWindow* getWindowById(wid_t id) override;
    const IWindow* getWindowById(wid_t id) const override;

    void addWindow(std::unique_ptr<IWindow> window) override;
    void removeWindow(wid_t id) override;

    void setParent(const IWindow* parent) override;

    ChildInfo getNextChildInfo() const override;

    bool update(const IRenderWindow* renderWindow, const sfm::Event& event) override;

protected:
    void drawChildren(IRenderWindow* renderWindow) override;

private:
    const IWindow* parent_;

    std::vector<std::unique_ptr<ABarButton>> windows_;

    unsigned gapSize_ = 2;

    mutable vec2i maxChildPosNow_;
};

class ColorBar;

class ColorButton : public ABarButton
{
public:
    ColorButton(std::shared_ptr<AChangeColorAction> action);

    void draw(IRenderWindow* renderWindow) override;
    bool update(const IRenderWindow* renderWindow, const Event& event) override;

    void setPos (vec2i pos ) override;
    void setSize(vec2u size) override;

    void setParent(const IWindow* parent) override;

protected:
    const ColorBar* parent_;

    std::unique_ptr<IRectangleShape> shape_;

    std::shared_ptr<AChangeColorAction> action_;

    size_t indexInColorBar_;
};

class ColorBar : public AShapedButtonsBar
{
public:
    ColorBar(vec2i pos, vec2u size); // TODO: connects to his parent in the future

    IWindow* getWindowById(wid_t id) override;
    const IWindow* getWindowById(wid_t id) const override;

    void addWindow(std::unique_ptr<IWindow> window) override;
    void removeWindow(wid_t id) override;

    void setParent(const IWindow* parent) override;

    ChildInfo getNextChildInfo() const override;

    // creating this function because getNextChildInfo is dumb.
    ChildInfo getChildInfo(size_t childIndex) const;

    bool update(const IRenderWindow* renderWindow, const Event& event) override;

protected:
    void drawChildren(IRenderWindow* renderWindow) override;
    void setSize(vec2u size);
    
private:
    const IBar* parent_;

    unsigned gapSize_ = 64;
    vec2u childSize_ = {32, 32};

    std::vector<std::unique_ptr<ColorButton>> windows_;

    mutable vec2i nextChildPos_ = {static_cast<int>(gapSize_), 0};
};

std::unique_ptr<IBar> createCommonInstrumentBar(std::shared_ptr<APropertiesMediator> mediator);

} // namespace ps

#endif // PLUGINS_PLUGIN_LIB_INSTRUMENTS_BAR_HPP