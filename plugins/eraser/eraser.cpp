#include "eraser.hpp"

#include "pluginLib/splineDraw/splineDrawButton.hpp"

using namespace ps;

class EraserButton : public SplineDrawButton
{
public:
    EraserButton(std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture);

    virtual void drawPoint(ICanvas* canvas, ILayer* layer, const vec2i& point) override;
};


EraserButton::EraserButton(std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture) 
    : SplineDrawButton(std::move(sprite), std::move(texture)) 
{
}

void EraserButton::drawPoint(ICanvas* canvas, psapi::ILayer* layer, const vec2i& point)
{
    assert(thicknessOption_);
    float thickness = thicknessOption_->getThickness();
    int drawingRange = static_cast<int>((thickness + 1) / 2);

    Color color = canvas->getCanvasBaseColor();

    for (int i = -drawingRange; i <= drawingRange; ++i)
    {
        for (int j = -drawingRange; j <= drawingRange; ++j)
        {
            vec2i pos = point + vec2i{i, j};
            layer->setPixel(vec2i{pos.x, pos.y}, color);
        }
    }
}

bool onLoadPlugin()
{
    return instrument_button_functions::instrumentButtonOnLoadPlugin<EraserButton>(
        "media/textures/eraser.png"
    );
}

void onUnloadPlugin()
{
    return;
}
