#include "instrumentBar.hpp" 

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

ChildInfo InstrumentBar::getNextChildInfo() const
{
    return ChildInfo{maxChildPosNow_ + vec2i{static_cast<int>(gapSize_), 0}, {0, 0}};
}

bool InstrumentBar::update(const IRenderWindow* renderWindow, const Event& event)
{
    vec2u renderWindowSize = renderWindow->getSize();

    setPos(vec2i{
            static_cast<int>(InstrumentOptionsTopLeftPos.x * static_cast<float>(renderWindowSize.x)),
            static_cast<int>(InstrumentOptionsTopLeftPos.y * static_cast<float>(renderWindowSize.y))});
    setSize(vec2u{
            static_cast<unsigned>(InstrumentOptionsSize.x * static_cast<float>(renderWindowSize.x)), 
            static_cast<unsigned>(InstrumentOptionsSize.y * static_cast<float>(renderWindowSize.y))});

    bool updatedChildren = bar_children_handler_funcs::updateChildren(renderWindow, event, windows_);

    if (updatedChildren)
        return true;

    if (event.type != Event::MouseButtonPressed)
        return false;

    // Turn off all buttons if pressed outside 
    for (auto& window : windows_)
        window->setState(ABarButton::State::Normal);
    
    return true;
}

void InstrumentBar::drawChildren(IRenderWindow* renderWindow)
{
    for (auto& window : windows_)
        window->draw(renderWindow);
}

// color button implementation

ColorButton::ColorButton(std::shared_ptr<AChangeColorAction> action) : action_(action)
{
    shape_ = IRectangleShape::create(0, 0);
    shape_->setFillColor(action_->getColor());
}

void ColorButton::draw(IRenderWindow* renderWindow)
{
    shape_->draw(renderWindow);
}

bool ColorButton::update(const IRenderWindow* renderWindow, const Event& event)
{
    if (!isActive_)
    {
        return false;
    }

    assert(parent_);

    State prevState = state_;
    bool stateIsUpdated = updateState(renderWindow, event);

    if (!stateIsUpdated)
        return false;

    // TODO: action should change the button not only send color to mediator
    if (action_ && state_ == State::Released && prevState != State::Released)
        action_->operator()();

    return true;
}

void ColorButton::setPos(vec2i pos)
{
    pos_ = pos;

    shape_->setPosition(pos_);
}

void ColorButton::setSize(vec2u size)
{
    size_ = size;

    shape_->setSize(size_);
}

// Color bar implementation

ColorBar::ColorBar(vec2i pos, vec2u size)
{
    shape_ = IRectangleShape::create(size.x, size.y);

    shape_->setFillColor({120, 120, 120, 255});

    ColorBar::setPos(pos);
    ColorBar::setSize(size);
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

ChildInfo ColorBar::getNextChildInfo() const
{
    nextChildPos_.y = pos_.y;

    ChildInfo result{nextChildPos_, vec2i{static_cast<int>(childSize_.x), static_cast<int>(childSize_.y)}};
    nextChildPos_ += vec2i{static_cast<int>(childSize_.x + gapSize_), 0};

    return result;
}

bool ColorBar::update(const IRenderWindow* renderWindow, const Event& event)
{
    if (!isActive_)
        return false;
    
    setPos(vec2i{
            static_cast<int>(InstrumentOptionsTopLeftPos.x * static_cast<float>(renderWindow->getSize().x)),
            static_cast<int>(InstrumentOptionsTopLeftPos.y * static_cast<float>(renderWindow->getSize().y))});
    setSize(vec2u{
            static_cast<unsigned>(InstrumentOptionsSize.x * static_cast<float>(renderWindow->getSize().x)), 
            static_cast<unsigned>(InstrumentOptionsSize.y * static_cast<float>(renderWindow->getSize().y))});

    bool updatedChildren = bar_children_handler_funcs::updateChildren(renderWindow, event, windows_);

    if (updatedChildren)
        return true;
    
    return false;
}

void ColorBar::drawChildren(IRenderWindow* renderWindow)
{
    for (auto& window : windows_)
        window->draw(renderWindow);
}

// Other functions implementation

std::unique_ptr<IBar> createCommonInstrumentBar(std::shared_ptr<APropertiesMediator> mediator)
{
    auto instrumentBar = std::make_unique<InstrumentBar>();

    Color colors[] = {
        {255, 0, 0, 255},
        {0, 255, 0, 255},
        {0, 0, 255, 255},
        {255, 255, 0, 255},
        {0, 255, 255, 255},
        {255, 0, 255, 255}
    };

    const size_t nButtons = sizeof(colors) / sizeof(colors[0]);

    vec2u size = {256, 16}; // TODO: CHANGE
    auto colorBar = std::make_unique<ColorBar>(instrumentBar->getNextChildInfo().pos, size);

    for (size_t i = 0; i < nButtons; ++i)
    {

        auto action = std::make_shared<ChangeFillColorAction>(colors[i], mediator);
        auto colorButton = std::make_unique<ColorButton>(action);
        colorButton->setParent(colorBar.get());

        ChildInfo info = colorBar->getNextChildInfo();
        colorButton->setPos(info.pos);
        colorButton->setSize(vec2u{static_cast<unsigned>(info.size.x), static_cast<unsigned>(info.size.y)});

        colorBar->addWindow(std::move(colorButton));
    }
    
    //instrumentBar->addWindow(std::move(colorBar));

    return colorBar;
}


} // namespace ps