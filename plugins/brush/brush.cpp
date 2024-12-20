#include "brush.hpp"
#include <string>
#include <cassert>

#include "api/api_sfm.hpp"
#include "api/api_photoshop.hpp"
#include "api/api_bar.hpp"
#include "api/api_canvas.hpp"

#include "pluginLib/bars/ps_bar.hpp"
#include "pluginLib/interpolation/include/interpolator.hpp"
#include "pluginLib/toolbar/toolbarButton.hpp"
#include "pluginLib/actions/actions.hpp"
#include "pluginLib/splineDraw/splineDrawButton.hpp"

#include <iostream>
#include <memory>

using namespace ps;

namespace
{

using namespace psapi;
using namespace psapi::sfm;

class BrushButton : public SplineDrawButton
{
public:
    BrushButton() = default;
    BrushButton(std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture);

    void drawPoint(ICanvas* canvas, ILayer* layer, const vec2i& point) override;
};

BrushButton::BrushButton(std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture) 
    : SplineDrawButton(std::move(sprite), std::move(texture)) 
{
}

void BrushButton::drawPoint(ICanvas* /* canvas */, ILayer* layer, const vec2i& point)
{
    assert(colorPalette_);
    assert(thicknessOption_);
    Color color = colorPalette_->getColor();
    float thickness = thicknessOption_->getThickness();
    int drawingRange = static_cast<int>((thickness + 1) / 2);

    for (int i = -drawingRange; i <= drawingRange; ++i)
    {
        for (int j = -drawingRange; j <= drawingRange; ++j)
        {
            vec2i pos = point + vec2i{i, j};
            layer->setPixel(vec2i{pos.x, pos.y}, color);
        }
    }
}

} // namespace anonymous

bool onLoadPlugin()
{
    return instrument_button_functions::instrumentButtonOnLoadPlugin<BrushButton>(
        "assets/textures/paintbrush.png");
}

void onUnloadPlugin()
{
    return;
}
