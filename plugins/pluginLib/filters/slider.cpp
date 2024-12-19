#include "slider.hpp"

#include <cassert>
#include <string>

namespace ps
{

NamedSlider::NamedSlider(const char* caption, float maxValue)
    : maxValue_(maxValue)
{
    captionFont_ = IFont::create();
    assert(captionFont_);
    captionFont_->loadFromFile("media/fonts/arial.ttf");
    
    caption_ = IText::create();
    caption_->setFont(captionFont_.get());

    caption_->setString(caption);
    caption_->setCharacterSize(SliderX::characterSize_);
}

void NamedSlider::setSlider(std::unique_ptr<SliderX> slider)
{
    slider_ = std::move(slider);
}

void NamedSlider::draw(IRenderWindow* renderWindow)
{
    caption_->draw(renderWindow);
    slider_->draw(renderWindow);
}

std::unique_ptr<IAction> NamedSlider::createAction(const IRenderWindow* renderWindow, const Event& event)
{
    return slider_->createAction(renderWindow, event);
}

float NamedSlider::getCurrentValue() const
{
    return slider_->getCurrentFullness() * maxValue_;
}

void NamedSlider::setPos(const vec2i& pos)
{
    caption_->setPos(vec2f{static_cast<float>(pos.x), static_cast<float>(pos.y)});

    IntRect textBounds = caption_->getGlobalBounds();

    const int offsetFromCaption = 5;

    slider_->setPos(vec2i{pos.x + static_cast<int>(textBounds.size.x) + offsetFromCaption, pos.y});
}

void NamedSlider::setSize(const vec2u& /* size */)
{
    assert(0 && "CAN'T SET SIZE TO SLIDER");
}

} // namespace ps