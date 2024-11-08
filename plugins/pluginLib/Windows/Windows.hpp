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

    void setParent(const IWindow* parent) override;

    void forceActivate() override;
    void forceDeactivate() override;

    bool isActive() const override;
    bool isWindowContainer() const override;

protected:
    wid_t id_ = kInvalidWindowId;

    const IWindow* parent_ = nullptr;

    vec2i pos_;
    vec2u size_;

    bool isActive_ = true;

protected:
    bool checkIsHovered(vec2i mousePos);
    bool updateIsPressed(const Event& event, bool prevPressedState, vec2i mousePos);
};

class ASpritedWindow : public AWindow
{
public:
    ASpritedWindow(
        vec2i pos, vec2u size, wid_t id, 
        std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture
    );

    void draw(IRenderWindow* renderWindow) override;

protected:
    std::unique_ptr<ISprite> sprite_;
    std::unique_ptr<ITexture> texture_;
};

class AWindowVector : public IWindowVector, public AWindow
{
public:
    AWindowVector(vec2i pos, vec2u size, wid_t id);

protected:
    bool updateChildren(const IRenderWindow* renderWindow, const sfm::Event& event);
    void drawChildren(IRenderWindow* renderWindow);
};

class ASpritedWindowVector : public AWindowVector
{
public:
    ASpritedWindowVector(
        vec2i pos, vec2u size, wid_t id, 
        std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture
    );

    void draw(IRenderWindow* renderWindow) override;

protected:
    std::unique_ptr<ISprite> sprite_;
    std::unique_ptr<ITexture> texture_;
};

class AWindowContainer : public IWindowContainer, public AWindow
{
public:
    AWindowContainer(vec2i pos, vec2u size, wid_t id) : AWindow(pos, size, id) {};

    IWindow* getWindowById(wid_t id) override = 0;
    const IWindow* getWindowById(wid_t id) const override = 0;

    bool isWindowContainer() const override { return true; }

    wid_t getId() const override { return AWindow::getId(); };

    vec2i getPos() const override { return AWindow::getPos(); };
    vec2u getSize() const override { return AWindow::getSize(); };

    void setParent(const IWindow* parent) override { AWindow::setParent(parent); };

    void forceActivate() override { AWindow::forceActivate(); };
    void forceDeactivate() override { AWindow::forceDeactivate(); };

    bool isActive() const override { return AWindow::isActive(); };
};

} // namespace ps

#endif // PLUGINS_PLUGINLIB_WINDOWS_WINDOWS_HPP