#include "optionsBar.hpp"

#include "api/api_bar.hpp"
#include "api/api_sfm.hpp"

#include "pluginLib/actions/actions.hpp"
#include "pluginLib/sfmHelpful/sfmHelpful.hpp"
#include "interfaceInfo/interfaceInfo.hpp"

#include <vector>
#include <memory>
#include <cassert>
#include <iostream>

using namespace psapi;
using namespace psapi::sfm;
using namespace ps;

class IIntRectGetterAction
{
public:
    virtual IntRect operator()() = 0;

    virtual ~IIntRectGetterAction() = default;
};

class GetUpperIntRectAction : public IIntRectGetterAction
{
public:
    IntRect operator()() override { return getToolOptionsUpIntRect(); }
};

class GetRightIntRectAction : public IIntRectGetterAction
{
public:
    IntRect operator()() override { return getToolOptionsRightIntRect(); }
};

class SubOptionsBar : public IOptionsBar
{
public:
    SubOptionsBar(wid_t id, std::unique_ptr<IIntRectGetterAction> intRectAction,
                  vec2i gapBetweenChildren, vec2i gapFromBoundaries, vec2u childMaxSize);

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

    std::unique_ptr<IIntRectGetterAction> intRectAction_;

    wid_t id_ = kInvalidWindowId;
    vec2i pos_ = {0, 0};
    vec2u size_ = {0, 0};
    bool isActive_ = true;
    const IWindow* parent_ = nullptr;

    std::unique_ptr<IRectangleShape> background_;

    vec2i gapBetweenChildren_ = {0, 0};
    vec2i gapFromBoundaries_ = {0, 0};

    vec2u maxChildSize_ = {128, 128};
};

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

private:
    wid_t id_ = kOptionsBarWindowId;
    bool isActive_ = false;
    const IWindow* parent_ = nullptr;

    SubOptionsBar upperBar_;
    SubOptionsBar rightBar_;
};

SubOptionsBar::SubOptionsBar(wid_t id, std::unique_ptr<IIntRectGetterAction> intRectAction,
                             vec2i gapBetweenChildren, vec2i gapFromBoundaries, vec2u childMaxSize)
    : intRectAction_(std::move(intRectAction)), id_(id),
      gapBetweenChildren_(gapBetweenChildren), gapFromBoundaries_(gapFromBoundaries), 
      maxChildSize_(childMaxSize)
{
    background_ = createShape(getCommonBarColor(), vec2u{1, 1});

    static const size_t nStandardWindows = 3;
    windows_.resize(nStandardWindows);
}

void SubOptionsBar::draw(IRenderWindow* renderWindow)
{
    background_->draw(renderWindow);

    for (auto& window : windows_)
    {
        if (!window)
            continue;

        window->draw(renderWindow);
    }
}

std::unique_ptr<IAction> SubOptionsBar::createAction(const IRenderWindow* renderWindow, const Event& event)
{
    return std::make_unique<UpdateCallbackAction<SubOptionsBar>>(*this, renderWindow, event);
}

void SubOptionsBar::setChildrenInfo()
{
    vec2i childPos = pos_ + gapFromBoundaries_;

    for (auto& window : windows_)
    {
        if (!window)
            continue;

        window->setPos(childPos);

        vec2u windowSize = window->getSize();
        window->setSize(vec2u{std::min(windowSize.x, maxChildSize_.x), 
                              std::min(windowSize.y, maxChildSize_.y)});
        windowSize = window->getSize();

        if (gapBetweenChildren_.x > 0)
            childPos.x += static_cast<int>(windowSize.x) + gapBetweenChildren_.x;
        
        if (gapBetweenChildren_.y > 0)
            childPos.y += static_cast<int>(windowSize.y) + gapBetweenChildren_.y;            
    }

#if 0
    if (windows_[0])
        windows_[0]->setPos({pos_.x + xShiftFromLeft, pos_.y + gap_});
    assert(windows_[1].get() == nullptr);
    assert(windows_[2].get() == nullptr);
    assert(windows_.size() == 3);
#endif
}

bool SubOptionsBar::update(const IRenderWindow* renderWindow, const Event& event)
{
    assert(intRectAction_);
    IntRect optionsRect = intRectAction_->operator()();
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

wid_t SubOptionsBar::getId() const { return id_; }

IWindow* SubOptionsBar::getWindowById(wid_t id) 
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

const IWindow* SubOptionsBar::getWindowById(wid_t id) const
{
    return const_cast<SubOptionsBar*>(this)->getWindowById(id);
}

vec2i SubOptionsBar::getPos() const { return pos_; }
vec2u SubOptionsBar::getSize() const { return size_; }

void SubOptionsBar::setSize(const vec2u& size)
{
    size_ = size;

    background_->setSize(size);
}

void SubOptionsBar::setPos(const vec2i& pos)
{
    pos_ = pos;
    background_->setPosition(pos);
}

void SubOptionsBar::setParent(const IWindow* parent) { parent_ = parent; }

void SubOptionsBar::forceActivate() { isActive_ = true; }
void SubOptionsBar::forceDeactivate() { isActive_ = false; }
bool SubOptionsBar::isActive() const { return isActive_; }
bool SubOptionsBar::isWindowContainer() const { return true; }

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

void SubOptionsBar::addWindow(std::unique_ptr<IWindow> window)
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

void SubOptionsBar::removeWindow(wid_t id)
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

void SubOptionsBar::removeAllOptions()
{
    windows_.clear();
    windows_.resize(0);
    windows_.resize(3);
}

// Options bar implementation

/*wid_t id, std::unique_ptr<IIntRectGetterAction> intRectAction,
                             vec2i gapBetweenChildren, vec2i gapFromBoundaries, vec2u childMaxSize*/
OptionsBar::OptionsBar()
    : upperBar_(kInvalidWindowId, std::make_unique<GetUpperIntRectAction>(), 
                {4, 0}, {4, 4}, {32, 0}),
      rightBar_(kInvalidWindowId, std::make_unique<GetRightIntRectAction>(), 
                {0, 4}, {8, 8}, {100, 512})
{
}

void OptionsBar::draw(IRenderWindow* renderWindow)
{
    upperBar_.draw(renderWindow);
    rightBar_.draw(renderWindow);
}

std::unique_ptr<IAction> OptionsBar::createAction(const IRenderWindow* renderWindow, const Event& event)
{

    return std::make_unique<UpdateCallbackAction<OptionsBar>>(*this, renderWindow, event);
}

bool OptionsBar::update(const IRenderWindow* renderWindow, const Event& event)
{
    AActionController* controller = getActionController();
    bool executedSomeone = controller->execute(upperBar_.createAction(renderWindow, event));
    executedSomeone |= controller->execute(rightBar_.createAction(renderWindow, event));

    return executedSomeone;
}

wid_t OptionsBar::getId() const { return id_; }

IWindow* OptionsBar::getWindowById(wid_t id) 
{
    if (id_ == id)
        return this;

    IWindow* searchResult = upperBar_.getWindowById(id);

    if (searchResult)
        return searchResult;
    
    searchResult = rightBar_.getWindowById(id);

    return searchResult;
}

const IWindow* OptionsBar::getWindowById(wid_t id) const
{
    return const_cast<OptionsBar*>(this)->getWindowById(id);
}

vec2i OptionsBar::getPos() const { assert(0 && "OPTIONS BAR HAVE NO POSITION, SPLITTED ON TWO"); }
vec2u OptionsBar::getSize() const { assert(0 && "OPTIONS BAR HAVE NO SIZE, SPLITTED ON TWO"); }

void OptionsBar::setSize(const vec2u& /* size */)
{
    assert(0 && "OPTIONS BAR HAVE NO SIZE -> CAN'T SET IT, SPLITTED ON TWO");
}

void OptionsBar::setPos(const vec2i& /* pos */)
{
    assert(0 && "OPTIONS BAR HAVE NO POS -> CAN'T SET IT, SPLITTED ON TWO");
}

void OptionsBar::setParent(const IWindow* parent) { parent_ = parent; }

void OptionsBar::forceActivate() { isActive_ = true; }
void OptionsBar::forceDeactivate() { isActive_ = false; }
bool OptionsBar::isActive() const { return isActive_; }
bool OptionsBar::isWindowContainer() const { return true; }

void OptionsBar::addWindow(std::unique_ptr<IWindow> window)
{
    switch (window->getId())
    {
        case kColorPaletteId:
            rightBar_.addWindow(std::move(window));
            break;
        case kThicknessBarId:
            upperBar_.addWindow(std::move(window));
            break;
        case kOpacityBarId:
            upperBar_.addWindow(std::move(window));
            break;
            
        default:
            std::cerr << "PUSHING UKNOWN WINDOW WITH ID - " << window->getId() << std::endl;
            rightBar_.addWindow(std::move(window));
            break;
    }
}

void OptionsBar::removeWindow(wid_t id)
{
    upperBar_.removeWindow(id);
    rightBar_.removeWindow(id);
}

void OptionsBar::removeAllOptions()
{
    upperBar_.removeAllOptions();
    rightBar_.removeAllOptions();
}

// Plugin specific functions
    
bool onLoadPlugin()
{
    getRootWindow()->addWindow(std::make_unique<OptionsBar>());

    return true;
}

void onUnloadPlugin()
{
    return;
}