#ifndef PLUGINS_PLUGINLIB_WINDOWS_WINDOWS_HPP
#define PLUGINS_PLUGINLIB_WINDOWS_WINDOWS_HPP

#include "api/api_photoshop.hpp"

namespace ps
{

using namespace psapi;
using namespace psapi::sfm;

class AWindow : public IWindow
{
public:
    explicit AWindow(vec2i pos = {0, 0}, vec2u size = {0, 0}, wid_t id = kInvalidWindowId);

    wid_t getId() const override;

    IWindow* getWindowById(wid_t id) override;
    const IWindow* getWindowById(wid_t id) const override;

    vec2i getPos() const override;
    vec2u getSize() const override;

    void setPos(const vec2i& pos) override;
    void setSize(const vec2u& size) override;

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
    virtual bool checkIsHovered(vec2i mousePos);
    virtual bool checkIsClicked(const Event& event, vec2i mousePos);
    virtual bool updateIsPressed(const Event& event, bool prevPressedState, vec2i mousePos);
};

#if 0
class AWindowContainer : public IWindowContainer
{
public:
    AWindowContainer(vec2i pos = {0, 0}, vec2u size = {0, 0}, wid_t id = kInvalidWindowId);

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
#endif

vec2i shrinkPosToBoundary(const vec2i& objectPos, const vec2u& objectSize, 
                          const vec2i& boundaryPos, const vec2u& boundarySize);

bool checkIsHovered(vec2i mousePos, const vec2i& pos, const vec2u& size);
bool updateIsPressed(const Event& event, bool prevPressedState, bool isHovered);
bool checkIsClicked(const Event& event, bool isHovered);

} // namespace ps

#endif // PLUGINS_PLUGINLIB_WINDOWS_WINDOWS_HPP