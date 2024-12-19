#ifndef PLUGINS_PLUGIN_LIB_SLIDER_SLIDER_HPP
#define PLUGINS_PLUGIN_LIB_SLIDER_SLIDER_HPP

#include "api/api_photoshop.hpp"
#include "pluginLib/bars/ps_bar.hpp"

namespace ps
{

using namespace psapi;
using namespace psapi::sfm;

class ISliderTitleAction
{
public:
    virtual std::string getSliderTitle() const = 0;

    virtual ~ISliderTitleAction() = default;
};
 
class SliderX : public ABarButton
{
public:
    SliderX() = default;
    SliderX(vec2i pos, vec2u size, wid_t id, 
            std::unique_ptr<IRectangleShape> fillColorShape,
            SpriteInfo normalSprite, SpriteInfo pressedSprite,
            vec2u maxFillColorShapeSize, vec2u gapFromPosToShape,
            std::unique_ptr<ISliderTitleAction> sliderTitleAction);
    
    std::unique_ptr<IAction> createAction(const IRenderWindow* renderWindow, const Event& event) override;
    bool update(const IRenderWindow* renderWindow, const Event& event);

    void draw(IRenderWindow* renderWindow) override;

    void setPos(const vec2i& pos) override;
    void setSize(const vec2u& size) override;

    float getCurrentFullness() const;

public:
    static const unsigned characterSize_ = 15;

private:
    vec2u fillColorShapeSize_;
    vec2u fillColorShapeMaxSize_;
    vec2u gapFromPosToShape_;

    std::unique_ptr<IRectangleShape> fillColorShape_;

    SpriteInfo normalSprite_;
    SpriteInfo pressedSprite_;

    std::unique_ptr<ISliderTitleAction> sliderTitleAction_;
};

} // namespace ps

#endif // PLUGINS_PLUGIN_LIB_SLIDER_SLIDER_HPP