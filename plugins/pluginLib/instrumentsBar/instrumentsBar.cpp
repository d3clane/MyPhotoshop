#include "instrumentsBar.hpp" 

#include <cassert>

// instruments bar implementation

namespace ps
{

InstrumentsBar::InstrumentsBar()
{
    id_ = kInstrumentsBarId;
}

IWindow* InstrumentsBar::getWindowById(wid_t id)
{
    if (id == id_)
        return this;

    for (auto& window : windows_)
        if (window->getId() == id)
            return window.get();
        
    return nullptr;
}

const IWindow* InstrumentsBar::getWindowById(wid_t id) const
{
    return const_cast<InstrumentsBar*>(this)->getWindowById(id);
}

void InstrumentsBar::addWindow(std::unique_ptr<IWindow> window)
{
    ABarButton* button = dynamic_cast<ABarButton*>(window.get());
    assert(button);

    windows_.push_back(std::unique_ptr<ABarButton>(button));
    window.release();

    assert(button->getPos().x >= maxChildPosNow_.x);

    maxChildPosNow_ = button->getPos() + vec2i{button->getSize().x + gapSize_, 0};
}

void InstrumentsBar::removeWindow(wid_t id)
{
    for (auto& window : windows_)
    {
        if (window->getId() == id)
        {
            windows_.erase(windows_.begin() + windows_.size() - 1);
            return;
        }
    }
}

ChildInfo InstrumentsBar::getNextChildInfo() const
{
    return ChildInfo{maxChildPosNow_ + vec2i{gapSize_, 0}, {0, 0}};
}

bool InstrumentsBar::update(const IRenderWindow* renderWindow, const Event& event)
{
    vec2u renderWindowSize = renderWindow->getSize();

    setSize({renderWindowSize.x * InstrumentOptionsSize.x,
             renderWindowSize.y * InstrumentOptionsSize.y});

    setPos ({InstrumentOptionsTopLeftPos.x * renderWindowSize.x, 
             InstrumentOptionsTopLeftPos.y * renderWindowSize.y});

    bool updatedChildren = bar_children_handler_funcs::updateChildren(renderWindow, event, windows_);

    if (updatedChildren)
        return true;

    if (event.type != Event::MouseButtonPressed)
        return false;

    // Turn off all buttons if pressed outside 
    for (auto& window : windows_)
        window->setState(ABarButton::State::Normal);
}

void InstrumentsBar::drawChildren(IRenderWindow* renderWindow)
{
    for (auto& window : windows_)
        window->draw(renderWindow);
}

// color button implementation

ColorButton::ColorButton(std::shared_ptr<AChangeColorAction> action) : action_(action)
{
    isActive_ = false;

    shape_->setFillColor(action_->getColor());
}

void ColorButton::draw(IRenderWindow* renderWindow)
{
    shape_->draw(renderWindow);
}

bool ColorButton::update(const IRenderWindow* renderWindow, const Event& event)
{
    if (!isActive_)
        return false;

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
    pos_ = pos;
    size_ = size;
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
    ColorButton* button = dynamic_cast<ColorButton*>(window.get());
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

    ChildInfo result{nextChildPos_, vec2i{childSize_.x, childSize_.y}};
    nextChildPos_ += vec2i{childSize_.x + gapSize_, 0};

    return result;
}

bool ColorBar::update(const IRenderWindow* renderWindow, const Event& event)
{
    bool updatedChildren = bar_children_handler_funcs::updateChildren(renderWindow, event, windows_);

    if (updatedChildren)
        return true;
}

void ColorBar::drawChildren(IRenderWindow* renderWindow)
{
    for (auto& window : windows_)
        window->draw(renderWindow);
}

// Other functions implementation

std::unique_ptr<InstrumentsBar> createCommonInstrumentBar(std::shared_ptr<APropertiesMediator> mediator)
{
    auto instrumentsBar = std::make_unique<InstrumentsBar>();

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
    auto colorBar = std::make_unique<ColorBar>(instrumentsBar->getNextChildInfo().pos, size);

    for (size_t i = 0; i < nButtons; ++i)
    {
        auto action = std::make_shared<ChangeFillColorAction>(colors[i], mediator);
        auto colorButton = std::make_unique<ColorButton>(action);

        ChildInfo info = colorBar->getNextChildInfo();
        colorButton->setPos(info.pos);
        colorButton->setSize(vec2u{info.size.x, info.size.y});

        colorBar->addWindow(std::move(colorButton));
    }

    instrumentsBar->addWindow(std::move(colorBar));

    return instrumentsBar;
}


} // namespace ps