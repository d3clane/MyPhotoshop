#include "api/api_bar.hpp"

// TODO: Dependency on plugin lib
#include "pluginLib/actions/actions.hpp"
#include "pluginLib/bars/ps_bar.hpp"
#include "pluginLib/sfmHelpful/sfmHelpful.hpp"
#include "pluginLib/slider/slider.hpp"

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

class ThicknessOption : public IThicknessOption
{
public:
    ThicknessOption();

    float getThickness() const override;
    void setThickness(float thickness) override;
    void draw(IRenderWindow* renderWindow) override;

    std::unique_ptr<IAction> createAction(const IRenderWindow* renderWindow, const Event& event) override;

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

private:
    wid_t id_ = kThicknessBarId;

    SliderX slider_;
};

class SliderPxTitleAction : public ISliderTitleAction
{
public:
    SliderPxTitleAction(ThicknessOption* thicknessOption) : thicknessOption_(thicknessOption) {}
    std::string getSliderTitle() const override;

private:
    ThicknessOption* thicknessOption_;
};

std::string SliderPxTitleAction::getSliderTitle() const
{
    float thickness = thicknessOption_->getThickness();

    return "Size: " + std::to_string(static_cast<int>(thickness)) + " px";
}


ThicknessOption::ThicknessOption()
{
    SpriteInfo slideNormal = createSprite("media/textures/sliderNormal.png");
    SpriteInfo slidePress = createSprite("media/textures/sliderPress.png");

    assert(slideNormal.sprite->getSize().x == slidePress.sprite->getSize().x);
    assert(slideNormal.sprite->getSize().y == slidePress.sprite->getSize().y);

    Color blueSliderColor = Color{74, 115, 145, 255};
    vec2u spritesOutlineWidth = {3, 2};
    vec2u maxFillColorSize = slideNormal.sprite->getSize() - 2 * spritesOutlineWidth;
    maxFillColorSize.x = 157;

    slider_ = SliderX{{0, 0}, slideNormal.sprite->getSize(), kInvalidWindowId, 
                      createShape(blueSliderColor, {1, 1}), 
                      std::move(slideNormal), std::move(slidePress),
                      maxFillColorSize, spritesOutlineWidth,
                      std::make_unique<SliderPxTitleAction>(this)};
}

float ThicknessOption::getThickness() const 
{ 
    const float maxThickness = 40;

    return slider_.getCurrentFullness() * maxThickness; 
}

void ThicknessOption::setThickness(float thickness) { assert(0); }

void ThicknessOption::draw(IRenderWindow* renderWindow)
{
    slider_.draw(renderWindow);
}

std::unique_ptr<IAction> ThicknessOption::createAction(const IRenderWindow* renderWindow, const Event& event)
{
    return slider_.createAction(renderWindow, event);
}

wid_t ThicknessOption::getId() const { return id_; }

IWindow* ThicknessOption::getWindowById(wid_t id)
{
    if (id_ == id)
        return this;

    return slider_.getWindowById(id);
}

const IWindow* ThicknessOption::getWindowById(wid_t id) const
{
    return const_cast<ThicknessOption*>(this)->getWindowById(id);
}

vec2i ThicknessOption::getPos() const { return slider_.getPos(); }
vec2u ThicknessOption::getSize() const { return slider_.getSize(); }

void ThicknessOption::setSize(const vec2u& size) { slider_.setSize(size); }
void ThicknessOption::setPos(const vec2i& pos) { slider_.setPos(pos); }
void ThicknessOption::setParent(const IWindow* parent) { slider_.setParent(parent); }

void ThicknessOption::forceActivate() { slider_.forceActivate(); }
void ThicknessOption::forceDeactivate() { slider_.forceDeactivate(); }
bool ThicknessOption::isActive() const { return slider_.isActive(); }
bool ThicknessOption::isWindowContainer() const { return false; }

std::unique_ptr<IThicknessOption> IThicknessOption::create()
{
    return std::make_unique<ThicknessOption>();
}
