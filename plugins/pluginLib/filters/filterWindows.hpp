#ifndef PLUGINS_PLUGIN_LIB_FILTER_WINDOWS_HPP
#define PLUGINS_PLUGIN_LIB_FILTER_WINDOWS_HPP

#include "api/api_memento.hpp"
#include "api/api_canvas.hpp"

#include "pluginLib/windows/windows.hpp"
#include "pluginLib/bars/ps_bar.hpp"

#include <vector>

namespace ps
{

// TODO: probably have to be IRootWindow
class FilterWindow : public IWindowContainer
{
public:
    FilterWindow(wid_t id, const char* filterWindowName);

    void draw(IRenderWindow* renderWindow) override;

    std::unique_ptr<IAction> createAction(const IRenderWindow* renderWindow, const Event& event) override;
    // returns false if render window is closed
    bool update(const IRenderWindow* renderWindow, const Event& event);

    wid_t getId() const override;
    IWindow* getWindowById(wid_t id) override;
    const IWindow* getWindowById(wid_t id) const override;

    vec2i getPos() const override;
    vec2u getSize() const override;

    void setSize(const vec2u& size) override;
    void setPos(const vec2i& pos) override;
    void setParent(const IWindow* parent) override;

    void forceActivate() override;
    void forceDeactivate() override;
    bool isActive() const override;

    bool isWindowContainer() const override;

    void addWindow(std::unique_ptr<IWindow> window) override;
    void removeWindow(wid_t id) override;

    bool closed() const;
    void close();

    vec2u getRenderWindowSize() const;

private:
    wid_t id_;
    bool isActive_ = true;

    std::unique_ptr<IRenderWindow> renderWindow_;
    std::vector<std::unique_ptr<IWindow>> windows_;
};

void addApplyButtons(FilterWindow* filterWindow);

class ApplyButtonsBar : public ABar
{
public:

private:
};

std::unique_ptr<FilterWindow> createSimpleFilterWindow(const char* name);

}

#endif // PLUGINS_PLUGIN_LIB_FILTER_WINDOWS_HPP
