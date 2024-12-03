#include "api/api_photoshop.hpp"
#include "api_impl/api_photoshop.hpp"

#include <cassert>
#include <iostream>


namespace psapi
{

class RootWindowAction : public IAction
{
public:
    bool execute(const Key& key) override;
    bool isUndoable(const Key& key) override;

    void addAction(std::unique_ptr<IAction> action);

private:
    std::vector<std::unique_ptr<IAction>> actions_;
};

bool RootWindowAction::execute(const Key& /*key*/)
{
    AActionController* actionController = getActionController();

    bool updatedSomeone = 0;
    for (auto& action : actions_)
        updatedSomeone |= actionController->execute(std::move(action));
    // Problem: can do something like action->execute(key);

    return updatedSomeone;
}

bool RootWindowAction::isUndoable(const Key& /* key */) 
{
    return false;
}

void RootWindowAction::addAction(std::unique_ptr<IAction> action)
{
    actions_.push_back(std::move(action));
}

void RootWindow::draw(IRenderWindow* renderWindow) 
{
    for (auto& window : windows_)
    {
        assert(window.get());
        window->draw(renderWindow);
    }
}

std::unique_ptr<IAction> RootWindow::createAction(const IRenderWindow* renderWindow,
                                                  const Event& event) 
{
    auto action = std::make_unique<RootWindowAction>();

    for (auto& window : windows_)
        action->addAction(window->createAction(renderWindow, event));

    return action;
}

wid_t RootWindow::getId() const 
{
    return kRootWindowId;
}

vec2i RootWindow::getPos() const 
{
    return {0, 0};
}

vec2u RootWindow::getSize() const 
{
    return renderWindow_->getSize();
}

void RootWindow::setParent(const IWindow* /* parent */) 
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
    return true;
}

void RootWindow::addWindow(std::unique_ptr<IWindow> window)
{
    windows_.push_back(std::move(window));
}

void RootWindow::removeWindow(wid_t id) 
{
    for (auto it = windows_.begin(); it != windows_.end(); it++) 
    {
        if ((*it)->getId() == id) 
        {
            windows_.erase(it);
            return;
        }
    }
}

IWindow* RootWindow::getWindowById(wid_t id)
{
    if (id == kRootWindowId)
        return static_cast<IWindow*>(this);
    
    for (auto& window : windows_)
    {
        IWindow* outWindow = window->getWindowById(id);
        if (outWindow)
            return outWindow;
    }

    return nullptr;
}

const IWindow* RootWindow::getWindowById(wid_t id) const
{
    return const_cast<RootWindow*>(this)->getWindowById(id);
}

void RootWindow::setSize(const vec2u& /*size*/)
{
    assert(false);
}

void RootWindow::setPos(const vec2i& /*pos*/)
{
    assert(false);
}

layer_id_t RootWindow::getUpperLayerId() const
{
    return maxLayerId_;
}

layer_id_t RootWindow::increaseLayerId()
{
    return ++maxLayerId_;
}

layer_id_t RootWindow::decreaseLayerId()
{
    assert(maxLayerId_ > 0);

    return --maxLayerId_;
}

IRenderWindow* RootWindow::getRenderWindow()
{
    return renderWindow_.get();
}

RootWindow* RootWindow::create(vec2u size)
{
    static RootWindow* rootWindow = new RootWindow;

    if (rootWindow->renderWindow_.get() == nullptr)
    {
        rootWindow->renderWindow_ = IRenderWindow::create(size.x, size.y, "Photoshop");
        rootWindow->renderWindow_->setFps(60);
    }

    return rootWindow;
}

IRootWindow* getRootWindow()
{
    return RootWindow::create(vec2u{0, 0});
}

vec2i getScreenSize()
{
    vec2u size = getRootWindow()->getSize();
    return vec2i{static_cast<int>(size.x), static_cast<int>(size.y)};
}

sfm::IntRect getCanvasIntRect()
{
    vec2i size = getScreenSize();
    
    sfm::IntRect rect;
    rect.pos  = {static_cast<int>     (static_cast<float>(size.x) * 0.1f),
                 static_cast<int>     (static_cast<float>(size.y) * 0.15f)};
    rect.size = {static_cast<unsigned>(static_cast<float>(size.x) * 0.8f),
                 static_cast<unsigned>(static_cast<float>(size.y) * 0.7f)};
    
    return rect;
}

sfm::IntRect getToolbarIntRect()
{
    vec2i size = getScreenSize();

    sfm::IntRect rect;
    rect.pos  = {0, 0};
    rect.size = {static_cast<unsigned>(static_cast<float>(size.x) * 0.055f), 
                 static_cast<unsigned>(size.y)};
    
    return rect;
}

sfm::IntRect getOptionsBarIntRect()
{
    vec2i size = getScreenSize();

    sfm::IntRect rect;
    rect.pos  = {static_cast<int>     (static_cast<float>(size.x) * 0.055f), 0};
    rect.size = {static_cast<unsigned>(static_cast<float>(size.x) * 0.945f), 
                 static_cast<unsigned>(static_cast<float>(size.y) * 0.055f)};
    
    return rect;
}

sfm::IntRect getInstrumentOptionsIntRect()
{
    vec2i size = getScreenSize();

    sfm::IntRect rect;

    rect.pos  = {static_cast<int>     (static_cast<float>(size.x) * 0.055f), 
                 static_cast<int>     (static_cast<float>(size.y) * 0.055f)};
    rect.size = {static_cast<unsigned>(static_cast<float>(size.x) * 0.945f), 
                 static_cast<unsigned>(static_cast<float>(size.y) * 0.055f)};
    
    return rect;
}

bool IWindowContainer::isWindowContainer() const 
{
    return true;
}

} // namespace psapi

