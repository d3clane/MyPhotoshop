#ifndef PLUGINS_PLUGIN_LIB_INSTRUMENTS_BAR_HPP
#define PLUGINS_PLUGIN_LIB_INSTRUMENTS_BAR_HPP

#include "pluginLib/bars/ps_bar.hpp"
#include "pluginLib/windows/windows.hpp"
#include "mediator.hpp"
#include "actions.hpp"

#include <vector>

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

    std::unique_ptr<IAction> createAction(const IRenderWindow* renderWindow, 
                                          const sfm::Event& event) override;

    bool update(const IRenderWindow* renderWindow, const sfm::Event& event);

    bool unPressAllButtons() override;

protected:
    void drawChildren(IRenderWindow* renderWindow) override;

private:
    const IWindow* parent_;

    std::vector<std::unique_ptr<ABarButton>> windows_;

    unsigned gapSize_ = 2;

    vec2i maxChildPosNow_;
};

class ColorBar;

class ColorButton : public ABarButton
{
public:
    ColorButton(std::shared_ptr<AChangeColorAction> action, size_t indexInColorBar);

    void draw(IRenderWindow* renderWindow) override;
    std::unique_ptr<IAction> createAction(const IRenderWindow* renderWindow, 
                                          const Event& event) override;

    bool update(const IRenderWindow* renderWindow, const Event& event);

    void setPos (const vec2i& pos ) override;
    void setSize(const vec2u& size) override;

    void setParent(const IWindow* parent) override;

protected:
    const IWindow* parent_;

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

    std::unique_ptr<IAction> createAction(const IRenderWindow* renderWindow, 
                                          const Event& event) override;

    bool update(const IRenderWindow* renderWindow, const Event& event);

    bool unPressAllButtons() override;

protected:
    void drawChildren(IRenderWindow* renderWindow) override;
    
private:
    const IWindow* parent_;

    unsigned gapSize_ = 64;
    vec2u childSize_ = {32, 32};

    std::vector<std::unique_ptr<ColorButton>> windows_;

    mutable vec2i nextChildPos_ = {static_cast<int>(gapSize_), 0};
};

template<typename MediatorType>
std::unique_ptr<IBar> createCommonInstrumentBar(std::shared_ptr<MediatorType> mediator)
{
    auto instrumentBar = std::make_unique<InstrumentBar>();

    Color colors[] = {
        {255, 0, 0, 255},
        {0, 255, 0, 255},
        {0, 0, 255, 255},
        {255, 255, 0, 255},
        {0, 255, 255, 255},
        {255, 0, 255, 255}
    };

    const size_t nButtons = sizeof(colors) / sizeof(colors[0]);

    //vec2u size= {256, 16}; // TODO: CHANGE
    auto colorBar = std::make_unique<ColorBar>(vec2i{0, 0}, vec2u{0, 0});

    for (size_t i = 0; i < nButtons; ++i)
    {
        auto action = std::make_shared<ChangeFillColorAction>(colors[i], mediator);
        auto colorButton = std::make_unique<ColorButton>(action, i);
        colorButton->setParent(colorBar.get());

        colorBar->addWindow(std::move(colorButton));
    }
    
    // TODO: 
    //instrumentBar->addWindow(std::move(colorBar));

    return colorBar;
}

} // namespace ps

#endif // PLUGINS_PLUGIN_LIB_INSTRUMENTS_BAR_HPP