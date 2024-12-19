#ifndef PLUGINS_PLUGIN_LIB_FILTERS_SLIDER_HPP
#define PLUGINS_PLUGIN_LIB_FILTERS_SLIDER_HPP

#include "pluginLib/slider/slider.hpp"

namespace ps
{

static const wid_t kRadiusSliderId = 72931;

class NamedSlider : public ABarButton
{
public:
    NamedSlider(const char* caption, float maxValue);

    void setSlider(std::unique_ptr<SliderX> slider);

    std::unique_ptr<IAction> createAction(const IRenderWindow* renderWindow, const Event& event) override;

    void draw(IRenderWindow* renderWindow) override;

    void setPos(const vec2i& pos) override;
    void setSize(const vec2u& size) override;

    float getCurrentValue() const;

private:
    std::unique_ptr<IText> caption_;
    std::unique_ptr<IFont> captionFont_;

    float maxValue_;

    std::unique_ptr<SliderX> slider_;
};

} // namespace ps

#endif // PLUGINS_PLUGIN_LIB_FILTERS_SLIDER_HPP