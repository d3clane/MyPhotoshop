#include "filterWindows.hpp"

#include "pluginLib/actions/actions.hpp"

#include <string>
#include <cassert>

namespace ps
{

FilterWindow::FilterWindow(wid_t id, const char* filterWindowName)
    : id_(id)
{
    static const vec2u renderWindowSize = {800, 600};

    renderWindow_ = IRenderWindow::create(renderWindowSize.x, renderWindowSize.y, filterWindowName);
}


void FilterWindow::draw(IRenderWindow* /* renderWindow */)
{ 
    if (!isActive_)
        return;

    renderWindow_->clear();

    for (auto& window : windows_)
        window->draw(renderWindow_.get());

    renderWindow_->display();
}

std::unique_ptr<IAction> FilterWindow::createAction(const IRenderWindow* /* renderWindow */, 
                                                    const Event& event)
{
    return std::make_unique<UpdateCallbackAction<FilterWindow>>(*this, renderWindow_.get(), event);
}

bool FilterWindow::update(const IRenderWindow* /* renderWindow */, const Event& /* event */)
{
    if (!isActive_)
        return false;
    // TODO: bad stuff... Let's just believe that no actions would be undoable and no bugs are here

    AActionController* actionController = getActionController();

    // TODO: govnokod -> create class for root window to call execute for it, not to run through children
    while (renderWindow_->isOpen())
    {
        Event event;
        while (renderWindow_->pollEvent(event))
        {
            if (event.type == psapi::sfm::Event::Closed)
            {
                renderWindow_->close();
                return false;
            }

            for (auto& window : windows_)
                actionController->execute(window->createAction(renderWindow_.get(), event));
        }

        for (auto& window : windows_)
            actionController->execute(window->createAction(renderWindow_.get(), event));
    }

    return true;
}

wid_t FilterWindow::getId() const
{
    return id_;
}

IWindow* FilterWindow::getWindowById(wid_t id)
{
    if (id == id_)
        return this;

    for (auto& window : windows_)
    {
        if (!window)
            continue;

        IWindow* searchRes = window->getWindowById(id);
        if (searchRes)
            return searchRes;
    }

    return nullptr;
}

const IWindow* FilterWindow::getWindowById(wid_t id) const
{
    return const_cast<FilterWindow*>(this)->getWindowById(id);
}

vec2i FilterWindow::getPos() const
{
    return {0, 0};
}

vec2u FilterWindow::getSize() const
{
    return renderWindow_->getSize();
}

void FilterWindow::setSize(const vec2u& /* size */)
{
    assert(0 && "CAN'T SET SIZE TO FILTER WINDOW");
}

void FilterWindow::setPos(const vec2i& /* pos */)
{
    assert(0 && "CAN'T SET POS TO FILTER WINDOW");
}

void FilterWindow::setParent(const IWindow* /* parent */)
{
    assert(0 && "CAN'T SET PARENT TO FILTER WINDOW");
}

void FilterWindow::forceActivate()
{
    isActive_ = true;
}

void FilterWindow::forceDeactivate()
{
    isActive_ = false;
}

bool FilterWindow::isActive() const
{
    return isActive_;
}

bool FilterWindow::isWindowContainer() const
{
    return true;
}

void FilterWindow::addWindow(std::unique_ptr<IWindow> window)
{
    windows_.push_back(std::move(window));
}

void FilterWindow::removeWindow(wid_t id)
{
    for (auto it = windows_.begin(); it != windows_.end(); ++it)
    {
        if ((*it)->getId() == id)
        {
            windows_.erase(it);
            return;
        }
    }
}

} // namespace ps