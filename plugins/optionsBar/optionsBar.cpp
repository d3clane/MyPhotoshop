#include "optionsBar.hpp"

#include "api/api_bar.hpp"
#include "api/api_sfm.hpp"

#include "pluginLib/actions/actions.hpp"

#include <vector>
#include <memory>
#include <cassert>
#include <iostream>

using namespace psapi;
using namespace psapi::sfm;
using namespace ps;

class OptionsBar : public IOptionsBar
{
public:
    OptionsBar();

    void draw(IRenderWindow* renderWindow) override;
    std::unique_ptr<IAction> createAction(const IRenderWindow* renderWindow, const Event& event) override;
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

    void removeAllOptions() override;

    void setChildrenInfo();

private:
    std::vector<std::unique_ptr<IWindow>> windows_;
    
    wid_t id_ = kOptionsBarWindowId;
    vec2i pos_ = {0, 0};
    vec2u size_ = {0, 0};
    bool isActive_ = true;
    const IWindow* parent_ = nullptr;

    std::unique_ptr<IRectangleShape> background_;

    int gap_ = 32;
    int xShiftFromLeft = 16;

    vec2u maxChildSize_ = {128, 128};
};

OptionsBar::OptionsBar()
{
    background_ = IRectangleShape::create(1, 1);
    background_->setFillColor(Color{120, 120, 120, 255});
    background_->setOutlineThickness(0);

    static const size_t nStandardWindows = 3;
    windows_.resize(nStandardWindows);
}

void OptionsBar::draw(IRenderWindow* renderWindow)
{
    background_->draw(renderWindow);

    for (auto& window : windows_)
    {
        if (!window)
            continue;

        window->draw(renderWindow);
    }
}

std::unique_ptr<IAction> OptionsBar::createAction(const IRenderWindow* renderWindow, const Event& event)
{
    return std::make_unique<UpdateCallbackAction<OptionsBar>>(*this, renderWindow, event);
}

void OptionsBar::setChildrenInfo()
{
    if (windows_[0])
        windows_[0]->setPos({pos_.x + xShiftFromLeft, pos_.y + gap_});
    assert(windows_[1].get() == nullptr);
    assert(windows_[2].get() == nullptr);
    assert(windows_.size() == 3);
}

bool OptionsBar::update(const IRenderWindow* renderWindow, const Event& event)
{
    IntRect optionsRect = getOptionsBarIntRect();
    setSize(optionsRect.size);
    setPos(optionsRect.pos);
    setChildrenInfo();

    AActionController* actionController = getActionController();
    assert(actionController);

    bool executedSomeone = false;
    for (auto& window : windows_)
    {
        if (!window)
            continue;

        executedSomeone |= actionController->execute(window->createAction(renderWindow, event));
    }

    return executedSomeone;
}

wid_t OptionsBar::getId() const { return id_; }

IWindow* OptionsBar::getWindowById(wid_t id) 
{
    if (id_ == id)
        return this;

    for (auto& window : windows_)
    {
        if (!window)
            continue;

        IWindow* searchResult = window->getWindowById(id);

        if (searchResult)
            return searchResult;
    }
        
    return nullptr;
}

const IWindow* OptionsBar::getWindowById(wid_t id) const
{
    return const_cast<OptionsBar*>(this)->getWindowById(id);
}

vec2i OptionsBar::getPos() const { return pos_; }
vec2u OptionsBar::getSize() const { return size_; }

void OptionsBar::setSize(const vec2u& size)
{
    size_ = size;

    background_->setSize(size);
}

void OptionsBar::setPos(const vec2i& pos)
{
    pos_ = pos;
    background_->setPosition(pos);
}

void OptionsBar::setParent(const IWindow* parent) { parent_ = parent; }

void OptionsBar::forceActivate() { isActive_ = true; }
void OptionsBar::forceDeactivate() { isActive_ = false; }
bool OptionsBar::isActive() const { return isActive_; }
bool OptionsBar::isWindowContainer() const { return true; }

static void addStandardWindow(std::vector<std::unique_ptr<IWindow>>& windows, 
                              std::unique_ptr<IWindow> window, size_t pos)
{
    static const size_t nStandardWindows = 3;
    assert(windows.size() >= nStandardWindows);

    if (windows[pos].get())
        windows.push_back(std::move(window));
    else
        windows[pos] = std::move(window);
}

void OptionsBar::addWindow(std::unique_ptr<IWindow> window)
{
    switch (window->getId())
    {
        case kColorPaletteId:
            addStandardWindow(windows_, std::move(window), 0);
            break;
        case kThicknessBarId:
            addStandardWindow(windows_, std::move(window), 1);
            break;
        case kOpacityBarId:
            addStandardWindow(windows_, std::move(window), 2);
            break;
            
        default:
            std::cerr << "PUSHING UKNOWN WINDOW WITH ID - " << window->getId() << std::endl;
            windows_.push_back(std::move(window));
            break;
    }
}

void OptionsBar::removeWindow(wid_t id)
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

void OptionsBar::removeAllOptions()
{
    windows_.clear();
    windows_.resize(0);
    windows_.resize(3);
}
    
bool onLoadPlugin()
{

    getRootWindow()->addWindow(std::make_unique<OptionsBar>());

    return true;
}

void onUnloadPlugin()
{
    return;
}