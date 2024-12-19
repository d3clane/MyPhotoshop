#include "slider.hpp"

#include "api/api_sfm.hpp"
#include "api/api_photoshop.hpp"

#include "pluginLib/actions/actions.hpp"
#include "interfaceInfo/interfaceInfo.hpp"

#include <algorithm>
#include <cassert>
#include <string>

using namespace psapi;
using namespace psapi::sfm;

namespace ps
{

namespace
{

std::unique_ptr<IFont> loadFont()
{
    std::unique_ptr<IFont> font_ = IFont::create();

    assert(font_);
    font_->loadFromFile("media/fonts/arial.ttf");

    return font_;
}

void drawTextUponSlider(IRenderWindow* renderWindow, const std::string& string,
                        const vec2f& sliderPos, const vec2u& sliderSize)
{
    if (string.empty())
        return;
    
    static std::unique_ptr<IFont> font_ = loadFont();
    assert(font_);

    static std::unique_ptr<IText> text_ = IText::create();
    assert(text_);

    text_->setFont(font_.get());
    text_->setCharacterSize(SliderX::characterSize_);
    text_->setString(string);

    text_->setPos({0.f, 0.f});
    IntRect bounds = text_->getGlobalBounds();

    // absolute magic constants. Idk how text works
    text_->setPos(vec2f{sliderPos.x + (sliderSize.x - bounds.size.x) / 2,
                        sliderPos.y + 0.285f * sliderSize.y - 0.43f * bounds.size.y});

    renderWindow->draw(text_.get());
}


} // namespace anonymous

SliderX::SliderX(vec2i pos, vec2u size, wid_t id, 
                std::unique_ptr<IRectangleShape> fillColorShape,
                SpriteInfo normalSprite, SpriteInfo pressedSprite,
                vec2u maxFillColorShapeSize, vec2u gapFromPosToShape,
                std::unique_ptr<ISliderTitleAction> sliderTitleAction)
    : fillColorShapeMaxSize_(maxFillColorShapeSize),
      gapFromPosToShape_(gapFromPosToShape),
      fillColorShape_(std::move(fillColorShape)), 
      normalSprite_(std::move(normalSprite)), pressedSprite_(std::move(pressedSprite)),
      sliderTitleAction_(std::move(sliderTitleAction))
{
    pos_ = pos;
    size_ = size;
    id_ = id;
    
    fillColorShapeSize_.x = 1;
    fillColorShapeSize_.y = maxFillColorShapeSize.y;
    fillColorShape_->setSize(fillColorShapeSize_);
}

std::unique_ptr<IAction> SliderX::createAction(const IRenderWindow* renderWindow, const Event& event)
{
    return std::make_unique<UpdateCallbackAction<SliderX>>(*this, renderWindow, event);
}

bool SliderX::update(const IRenderWindow* renderWindow, const Event& event)
{
    if (!isActive_)
        return false;

    bool stateIsUpdated = updateState(renderWindow, event);

    if (state_ == State::Released)
    {
        state_ = State::Normal;
        return stateIsUpdated;
    }
    if (state_ != State::Press)
        return stateIsUpdated; 

    vec2i mousePosition = Mouse::getPosition(renderWindow);
    vec2f shapePosition = fillColorShape_->getPosition();

    fillColorShapeSize_.x = std::clamp(static_cast<unsigned>((float)mousePosition.x - shapePosition.x), 
                                       0u, fillColorShapeMaxSize_.x);

    fillColorShape_->setSize(fillColorShapeSize_);
    
    return stateIsUpdated;
}

void SliderX::setPos(const vec2i& pos)
{
    ABarButton::setPos(pos);

    normalSprite_.sprite->setPosition(pos_.x, pos_.y);
    pressedSprite_.sprite->setPosition(pos_.x, pos_.y);

    fillColorShape_->setPosition(vec2f{(float)pos_.x, (float)pos_.y} +
                                 vec2f{(float)gapFromPosToShape_.x, (float)gapFromPosToShape_.y});
}

void SliderX::setSize(const vec2u& size)
{
    ABarButton::setSize(size);
    
    fprintf(stderr, "CAN'T SET SLIDER SIZE RN\n");
}
    
void SliderX::draw(IRenderWindow* renderWindow)
{
    if (!isActive_)
        return;

    switch (state_)
    {
        case State::Normal:
        case State::Hover:
        case State::Released:
            renderWindow->draw(normalSprite_.sprite.get());
            break;
        case State::Press:
            renderWindow->draw(pressedSprite_.sprite.get());
            break;

        default:
            assert(0);
            break;
    }

    renderWindow->draw(fillColorShape_.get());

    drawTextUponSlider(renderWindow, sliderTitleAction_->getSliderTitle(), 
                       fillColorShape_->getPosition(), fillColorShapeMaxSize_);
}

float SliderX::getCurrentFullness() const
{
    return static_cast<float>(fillColorShapeSize_.x) / static_cast<float>(fillColorShapeMaxSize_.x);
}

} // namespace ps