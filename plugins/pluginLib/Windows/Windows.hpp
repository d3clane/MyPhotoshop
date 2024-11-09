#ifndef PLUGINS_PLUGINLIB_WINDOWS_WINDOWS_HPP
#define PLUGINS_PLUGINLIB_WINDOWS_WINDOWS_HPP

#include "api/api_photoshop.hpp"

namespace ps
{

using namespace psapi;
using namespace psapi::sfm;

class AWindow : IWindow
{
public:
    AWindow(vec2i pos, vec2u size, wid_t id);

    wid_t getId() const override;

    IWindow* getWindowById(wid_t id) override;
    const IWindow* getWindowById(wid_t id) const override;

    vec2i getPos() const override;
    vec2u getSize() const override;

    void forceActivate() override;
    void forceDeactivate() override;

    bool isActive() const override;
    bool isWindowContainer() const override;

    void setParent(const IWindow* parent) override;

protected:
    wid_t id_ = kInvalidWindowId;

    vec2i pos_;
    vec2u size_;

    bool isActive_ = true;

    const IWindow* parent_ = nullptr;

protected:
    bool checkIsHovered(vec2i mousePos);
    bool updateIsPressed(const Event& event, bool prevPressedState, vec2i mousePos);
};

class AWindowVector : public IWindowVector, public AWindow
{
public:
    AWindowVector(vec2i pos, vec2u size, wid_t id);

protected:
    bool updateChildren(const IRenderWindow* renderWindow, const sfm::Event& event);
    void drawChildren(IRenderWindow* renderWindow);
};

class AWindowContainer : public IWindowContainer, public AWindow
{
public:
    AWindowContainer(vec2i pos, vec2u size, wid_t id);

    IWindow* getWindowById(wid_t id) override = 0;
    const IWindow* getWindowById(wid_t id) const override = 0;

    bool isWindowContainer() const override;

    wid_t getId() const override;

    vec2i getPos() const override;
    vec2u getSize() const override;

    void setParent(const IWindow* parent) override;

    void forceActivate() override;
    void forceDeactivate() override;

    bool isActive() const override;
};

vec2i shrinkPosToBoundary(const vec2i& objectPos, const vec2u& objectSize, 
                          const vec2i& boundaryPos, const vec2u& boundarySize);

} // namespace ps

#endif // PLUGINS_PLUGINLIB_WINDOWS_WINDOWS_HPP