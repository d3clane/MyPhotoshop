#include "api/api_bar.hpp"

// TODO: Dependency on plugin lib
#include "pluginLib/actions/actions.hpp"
#include "pluginLib/bars/ps_bar.hpp"
#include "pluginLib/sfmHelpful/sfmHelpful.hpp"
#include "pluginLib/scrollbar/scrollbar.hpp"

#include <vector>
#include <memory>
#include <cassert>

using namespace ps;
using namespace psapi;
using namespace psapi::sfm;

class ColorButton : public ABarButton
{
public:
    ColorButton(Color color);
    void draw(IRenderWindow* renderWindow) override;
    std::unique_ptr<IAction> createAction(const IRenderWindow* renderWindow, 
                                          const Event& event) override;

    bool update(const IRenderWindow* renderWindow, const Event& event);

    void setPos (const vec2i& pos ) override;
    void setSize(const vec2u& size) override;

    void setParent(const IWindow* parent) override;

    Color getColor() const;

protected:
    const IWindow* parent_;

    std::unique_ptr<IRectangleShape> shape_;
};

class ColorPalette : public IColorPalette
{
public:
    ColorPalette();

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

    Color getColor() const override;
    void setColor(const Color &color) override;

    void setChildrenInfo();

private:
    std::vector<std::unique_ptr<ColorButton>> colors_;

    Color activatedColor_;

    wid_t id_ = kColorPaletteId;
    vec2i pos_ = {0, 0};
    vec2u size_ = {0, 0};
    bool isActive_ = true;
    const IWindow* parent_ = nullptr;
};

ColorButton::ColorButton(Color color)
{
    shape_ = IRectangleShape::create(1, 1);

    shape_->setFillColor(color);
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

    if (state_ == State::Released && prevState != State::Released)
        return true;

    return false;
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

Color ColorButton::getColor() const { return shape_->getFillColor(); }

std::unique_ptr<IColorPalette> IColorPalette::create()
{
    return std::make_unique<ColorPalette>();
}

ColorPalette::ColorPalette()
{
    static const size_t nStandardColors = 9;

    for (size_t colorId = 0; colorId < nStandardColors; ++colorId)
    {
        Color color = Color::getStandardColor(static_cast<Color::Type>(colorId));
        
        auto colorButton = std::make_unique<ColorButton>(color);
        colorButton->setParent(this);
        colors_.push_back(std::move(colorButton));
    }

    setChildrenInfo();

    activatedColor_ = colors_[0]->getColor();
    colors_[0]->setState(IBarButton::State::Released);
}

void ColorPalette::draw(IRenderWindow* renderWindow)
{
    if (!isActive_)
        return;

    for (auto& color : colors_)
        color->draw(renderWindow);
}

void ColorPalette::setChildrenInfo()
{
    vec2u childSize = {64, 64};

    int gapSize = 16;
    int childIndex = 0;
    for (auto& color : colors_)
    {
        color->setPos(vec2i{ pos_.x + gapSize, 
                             gapSize + pos_.y + (childIndex * (static_cast<int>(childSize.y) + gapSize)) });
        color->setSize(childSize);
        childIndex++;
    }
}

std::unique_ptr<IAction> ColorPalette::createAction(const IRenderWindow* renderWindow,
                                               const sfm::Event& event)
{
    return std::make_unique<UpdateCallbackAction<ColorPalette>>(*this, renderWindow, event);
}

bool ColorPalette::update(const IRenderWindow* renderWindow, const sfm::Event& event) 
{
    AActionController* actionController = getActionController();
    bool updatedSomeone = false;

    static const size_t invalidPos = static_cast<size_t>(-1);
    size_t lastReleasedButtonPos = invalidPos;
    size_t childrenSize = colors_.size();
    for (size_t i = 0; i < childrenSize; ++i)
    {
        IBarButton::State state = colors_[i]->getState();
        updatedSomeone |= actionController->execute(colors_[i]->createAction(renderWindow, event));

        if (colors_[i]->getState() == IBarButton::State::Released && 
            state != IBarButton::State::Released)
        {
            lastReleasedButtonPos = i;
        }
    }

    if (lastReleasedButtonPos != invalidPos)
    {
        activatedColor_ = colors_[lastReleasedButtonPos]->getColor();
        for (size_t i = 0; i < childrenSize; ++i)
        {
            if (i == lastReleasedButtonPos)
                continue;
            colors_[i]->setState(IBarButton::State::Normal);
        }
    }

    return updatedSomeone;
}

void ColorPalette::setParent(const IWindow* parent)
{
    parent_ = parent;
}

wid_t ColorPalette::getId() const { return id_; }

IWindow* ColorPalette::getWindowById(wid_t id)
{
    if (id_ == id)
        return this;

    for (auto& window : colors_)
    {
        IWindow* searchResult = window->getWindowById(id);

        if (searchResult)
            return searchResult;
    }
        
    return nullptr;
}

const IWindow* ColorPalette::getWindowById(wid_t id) const
{
    return const_cast<ColorPalette*>(this)->getWindowById(id);
}

vec2i ColorPalette::getPos() const
{
    return pos_;
}

vec2u ColorPalette::getSize() const
{
    return size_;
}

void ColorPalette::setSize(const vec2u& size)
{
    size_ = size;

    setChildrenInfo();
}

void ColorPalette::setPos(const vec2i& pos)
{
    pos_ = pos;

    setChildrenInfo();
}

void ColorPalette::forceActivate() { isActive_ = true; }

void ColorPalette::forceDeactivate() { isActive_ = false; }
bool ColorPalette::isActive() const { return isActive_; }
bool ColorPalette::isWindowContainer() const { return false; }

Color ColorPalette::getColor() const { return activatedColor_; }

void ColorPalette::setColor(const Color& color) 
{
    activatedColor_ = color;
    
    for (auto& colorButton : colors_)
    {
        colorButton->setState(IBarButton::State::Normal);

        if (colorButton->getColor() == color)
            colorButton->setState(IBarButton::State::Released);
    }
}

// Size palette implementation

#if 0
class ThicknessOption : public IThicknessOption
{
public:

    float getThickness() const override;
    void setThickness(float thickness) override;
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

    Color getColor() const override;
    void setColor(const Color &color) override;

    void setChildrenInfo();

private:
    std::vector<std::unique_ptr<ColorButton>> colors_;

    Color activatedColor_;

    wid_t id_ = kColorPaletteId;
    vec2i pos_ = {0, 0};
    vec2u size_ = {0, 0};
    bool isActive_ = true;
    const IWindow* parent_ = nullptr;
};
#endif