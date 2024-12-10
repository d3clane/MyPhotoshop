#ifndef PLUGINS_PLUGIN_LIB_SPLINE_DRAW_SPLINE_DRAW_BUTTON_HPP
#define PLUGINS_PLUGIN_LIB_SPLINE_DRAW_SPLINE_DRAW_BUTTON_HPP

#include "pluginLib/bars/ps_bar.hpp"
#include "pluginLib/instrumentBar/mediator.hpp"
#include "pluginLib/instrumentBar/instrumentBar.hpp"
#include "pluginLib/toolbar/toolbarButton.hpp"
#include "pluginLib/interpolation/include/interpolator.hpp"
#include "api/api_canvas.hpp"

namespace ps
{

using MediatorType = AFillPropertiesMediator;

class SplineDrawButton : public AInstrumentButton<MediatorType> 
{
public:
    SplineDrawButton() = default;
    SplineDrawButton(std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture);

    std::unique_ptr<IAction> createAction(const IRenderWindow* renderWindow, 
                                          const Event& event) override;

    bool update(const IRenderWindow* renderWindow, const Event& event);
    void draw(IRenderWindow* renderWindow) override;

    virtual void drawPoint(psapi::ICanvas* canvas, psapi::ILayer* layer, const vec2i& point) = 0;

protected:
    Interpolator interpolator_;

    bool drawTrace(ICanvas* canvas);
};

} // namespace ps

#endif // PLUGINS_PLUGIN_LIB_SPLINE_DRAW_SPLINE_DRAW_BUTTON_HPP