#include "api/api_photoshop.hpp"

#include <cassert>

namespace psapi
{

class RootWindow : public IWindowVector {
public:
    void draw  (      IRenderWindow* renderWindow) override;
    bool update(const IRenderWindow* renderWindow,
                        const Event& event)           override;
    int64_t getId() const override;

    vec2i getPos()  const          override;
    vec2u getSize() const          override;
    void setParent(const IWindow* parent) override;
    void forceActivate()                  override;
    void forceDeactivate()                override;

    bool isActive() const override;

private:
    wid_t id_;
    bool isActive_ = true;
};

void RootWindow::draw(IRenderWindow* renderWindow) 
{
    for (auto& window : windows_)
        window->draw(renderWindow);
}

bool RootWindow::update(const IRenderWindow* renderWindow,
                        const Event& event) 
{
    bool updatedSomeone = false;

    for (auto& window : windows_)
        updatedSomeone |= window->update(renderWindow, event);

    return updatedSomeone;
}

int64_t RootWindow::getId() const {
    return kRootWindowId;
}

vec2i RootWindow::getPos() const {
    return {0, 0};
}

vec2u RootWindow::getSize() const {
    return {0, 0};
}

void RootWindow::setParent(const IWindow* parent) 
{
    assert(false);
}

void RootWindow::forceActivate() 
{
    assert(false);
}

void RootWindow::forceDeactivate() 
{
    assert(false);
}

bool RootWindow::isActive() const
{
    return isActive_;
}

IWindow::~IWindow() = default;

IWindowContainer* getRootWindow()
{
    static RootWindow rootWindow;
    return &rootWindow;
}

bool IWindowContainer::isWindowContainer() const {
    return true;
}

bool IWindowVector::isWindowContainer() const {
    return true;
}

void IWindowVector::addWindow(std::unique_ptr<IWindow> window) {
    windows_.push_back(std::move(window));
}

void IWindowVector::removeWindow(wid_t id) {
    for (auto it = windows_.begin(); it != windows_.end(); it++) {
        if ((*it)->getId() == id) {
            windows_.erase(it);
            return;
        }
    }
}

IWindow* IWindowVector::getWindowById(wid_t id) {
    return const_cast<IWindow*>(static_cast<const IWindowVector*>(this)->getWindowById(id));
}

const IWindow* IWindowVector::getWindowById(wid_t id) const {
    if (id == kInvalidWindowId) {
        return nullptr;
    }

    for (const auto& window : windows_) {
        IWindow* result = window->getWindowById(id);
        if (result) {
            return result;
        }
    }
    
    return nullptr;
}

} // namespace psapi