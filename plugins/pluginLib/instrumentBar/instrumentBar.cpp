#include "instrumentBar.hpp" 
#include "pluginLib/actions/actions.hpp"

#include <cassert>
#include <iostream>

// instrument bar implementation

namespace ps
{

InstrumentBar::InstrumentBar()
{
    id_ = kInstrumentBarId;
}

IWindow* InstrumentBar::getWindowById(wid_t id)
{
    if (id == id_)
        return this;

    for (auto& window : windows_)
        if (window->getId() == id)
            return window.get();
        
    return nullptr;
}

const IWindow* InstrumentBar::getWindowById(wid_t id) const
{
    return const_cast<InstrumentBar*>(this)->getWindowById(id);
}

void InstrumentBar::addWindow(std::unique_ptr<IWindow> window)
{
    ABarButton* button = static_cast<ABarButton*>(window.get());
    assert(button);

    windows_.push_back(std::unique_ptr<ABarButton>(button));
    window.release();

    assert(button->getPos().x >= maxChildPosNow_.x);

    maxChildPosNow_ = button->getPos() + vec2i{static_cast<int>(button->getSize().x + gapSize_), 0};
}

void InstrumentBar::removeWindow(wid_t id)
{
    for (auto& window : windows_)
    {
        if (window->getId() == id)
        {
            windows_.erase(windows_.begin() + static_cast<int>(windows_.size()) - 1);
            return;
        }
    }
}

void InstrumentBar::setParent(const IWindow* parent)
{
    parent_ = parent;
}

// creating action
class InstrumentBarAction : public IAction
{
public:
    InstrumentBarAction(const IRenderWindow* renderWindow, const Event& event)
        : renderWindow_(renderWindow), event_(event) {}

    bool execute(const Key& key);

private:
    const IRenderWindow* renderWindow_;
    Event event_;
};

std::unique_ptr<IAction> InstrumentBar::createAction(const IRenderWindow* renderWindow, 
                                                     const sfm::Event& event) 
{
    return std::make_unique<UpdateCallbackAction<InstrumentBar>>(*this, renderWindow, event);
}


bool InstrumentBar::update(const IRenderWindow* renderWindow, const Event& event)
{
    if (!isActive_)
        return false;

    IntRect instrumentBarInfo = getInstrumentOptionsIntRect();
    
    setPos (instrumentBarInfo.pos);
    setSize(instrumentBarInfo.size);

    std::unique_ptr<IAction> updateChildrenAction = 
        bar_children_handler_funcs::createUpdateChildrenAction(renderWindow, event, windows_);
    
    bool updatedChildren = getActionController()->execute(std::move(updateChildrenAction));

    if (updatedChildren)
        return true;

    if (event.type != Event::MouseButtonPressed)
        return false;

    // TODO: ??? I don't remember why did I write this
    // Turn off all buttons if pressed outside 
    for (auto& window : windows_)
        window->setState(ABarButton::State::Normal);
    
    return true;
}

bool InstrumentBar::unPressAllButtons()
{
    return bar_children_handler_funcs::unPressAllButtons(windows_);
}

void InstrumentBar::drawChildren(IRenderWindow* renderWindow)
{
    for (auto& window : windows_)
        window->draw(renderWindow);
}

// color button implementation

ColorButton::ColorButton(std::shared_ptr<AChangeColorAction> action, size_t indexInColorBar) 
    : action_(action), indexInColorBar_(indexInColorBar)
{
    shape_ = IRectangleShape::create(0, 0);
    shape_->setFillColor(action_->getColor());
}

void ColorButton::setParent(const IWindow* parent)
{
    parent_ = parent;
}

void ColorButton::draw(IRenderWindow* renderWindow)
{
    shape_->draw(renderWindow);
}

std::unique_ptr<IAction> ColorButton::createAction(const IRenderWindow* renderWindow, 
                                        const Event& event)
{
    return std::make_unique<UpdateCallbackAction<ColorButton>>(*this, renderWindow, event);
}

bool ColorButton::update(const IRenderWindow* renderWindow, const Event& event)
{
    if (!isActive_)
        return false;

    State prevState = state_;
    bool stateIsUpdated = updateState(renderWindow, event);

    if (!stateIsUpdated)
        return false;

    // TODO: action should change the button not only send color to mediator
    if (action_ && state_ == State::Released && prevState != State::Released)
        action_->operator()();

    return true;
}

void ColorButton::setPos(const vec2i& pos)
{
    pos_ = pos;

    shape_->setPosition(pos_);
}

void ColorButton::setSize(const vec2u& size)
{
    size_ = size;

    shape_->setSize(size_);
}

// Color bar implementation

ColorBar::ColorBar(vec2i pos, vec2u size)
{
    shape_ = IRectangleShape::create(size.x, size.y);

    shape_->setFillColor({120, 120, 120, 255});

    setPos(pos);
    setSize(size);
}

void ColorBar::setParent(const IWindow* parent)
{
    parent_ = parent;
}

IWindow* ColorBar::getWindowById(wid_t id)
{
    if (id_ == id)
        return this;

    for (auto& window : windows_)
        if (window->getId() == id)
            return window.get();
        
    return nullptr;
}

const IWindow* ColorBar::getWindowById(wid_t id) const
{
    return const_cast<ColorBar*>(this)->getWindowById(id);
} 

void ColorBar::addWindow(std::unique_ptr<IWindow> window)
{
    ColorButton* button = static_cast<ColorButton*>(window.get());
    assert(button);

    windows_.push_back(std::unique_ptr<ColorButton>(button));
    window.release();
}

void ColorBar::removeWindow(wid_t id)
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

#if 0
ChildInfo ColorBar::getNextChildInfo() const
{
    nextChildPos_.y = pos_.y;

    ChildInfo result{nextChildPos_, vec2i{static_cast<int>(childSize_.x), static_cast<int>(childSize_.y)}};
    nextChildPos_ += vec2i{static_cast<int>(childSize_.x + gapSize_), 0};

    return result;
}

ChildInfo ColorBar::getChildInfo(size_t childIndex) const
{
    ChildInfo info;

    info.size = vec2i{ static_cast<int>(childSize_.x), static_cast<int>(childSize_.y) };
    
    int shift = static_cast<int>(static_cast<unsigned>(childIndex) * (childSize_.x + gapSize_) + gapSize_);

    if (size_.y < size_.x)
    {
        info.pos.x = pos_.x + shift;
        info.pos.y = pos_.y + static_cast<int>(gapSize_);
    }
    else
    {
        info.pos.y = pos_.y + shift;
        info.pos.x = pos_.x + static_cast<int>(gapSize_);
    }

    return info;
}
#endif

std::unique_ptr<IAction> ColorBar::createAction(const IRenderWindow* renderWindow, 
                                                const sfm::Event& event)
{
    return std::make_unique<UpdateCallbackAction<ColorBar>>(*this, renderWindow, event);
}

bool ColorBar::update(const IRenderWindow* renderWindow, const Event& event)
{
    if (!isActive_)
        return false;
    
    IntRect instrumentBarRect = getInstrumentOptionsIntRect();
    setPos(instrumentBarRect.pos);
    setSize(instrumentBarRect.size);

    std::unique_ptr<IAction> updateChildrenAction = 
        bar_children_handler_funcs::createUpdateChildrenAction(renderWindow, event, windows_);

    bool updatedChildren = getActionController()->execute(std::move(updateChildrenAction));

    if (updatedChildren)
        return true;
    
    return false;
}

bool ColorBar::unPressAllButtons()
{
    return bar_children_handler_funcs::unPressAllButtons(windows_);
}

void ColorBar::drawChildren(IRenderWindow* renderWindow)
{
    for (auto& window : windows_)
    {
        window->draw(renderWindow);
        #if 0
        finishButtonDraw(renderWindow, window.get());
        #endif
    }
}

} // namespace ps