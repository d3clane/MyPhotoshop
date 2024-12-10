#include "splineDrawButton.hpp"

#include "pluginLib/actions/actions.hpp"

#include <iostream>

using namespace psapi;
using namespace psapi::sfm;

namespace ps
{

SplineDrawButton::SplineDrawButton(std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture)
{
    mainSprite_ = std::move(sprite);
    mainTexture_ = std::move(texture);
}

std::unique_ptr<IAction> SplineDrawButton::createAction(const IRenderWindow* renderWindow, 
                                                   const Event& event)
{
    return std::make_unique<UpdateCallbackAction<SplineDrawButton>>(*this, renderWindow, event);
}

bool SplineDrawButton::update(const IRenderWindow* renderWindow, const Event& event)
{
    bool updatedState = updateState(renderWindow, event);

#if 0
    getActionController()->execute(
        instrument_button_functions::createActionInstrumentBar(
            instrumentBar_.get(), state_, renderWindow, event
        )
    );
#endif

    if (state_ != State::Released)
        return updatedState;
    
    // TODO: may be really slow
    ICanvas* canvas = static_cast<ICanvas*>(getRootWindow()->getWindowById(kCanvasWindowId));

    if (!canvas)
    {
        std::cerr << "CANVAS NOT FOUND!\n";
        assert(0);
    }

    if (!canvas->isPressedLeftMouseButton())
    {
        interpolator_.clear();
        return true;
    }

    if (drawTrace(canvas))
        interpolator_.popFront();

    vec2i mousePos = canvas->getMousePosition();
    interpolator_.pushBack({mousePos.x, mousePos.y});

    return updatedState;
}

void SplineDrawButton::draw(IRenderWindow* renderWindow)
{
    if (!isActive_)
        return;

    ASpritedBarButton::draw(renderWindow, parent_);

    //instrument_button_functions::drawInstrumentBar(instrumentBar_.get(), renderWindow);
}

bool SplineDrawButton::drawTrace(ICanvas* canvas)
{
    if (!interpolator_.isPossibleToDraw())
        return false;

    size_t activeLayerIndex = canvas->getActiveLayerIndex();
    ILayer* activeLayer = canvas->getLayer(activeLayerIndex);

    for (double interpolatedPos = 1; interpolatedPos < 2; interpolatedPos += 0.01)
    {
        vec2d interpolatedPoint = interpolator_[interpolatedPos];
        drawPoint(canvas, activeLayer, 
                  vec2i{static_cast<int>(interpolatedPoint.x), static_cast<int>(interpolatedPoint.y)});
    }

    return true;
}

} // namespace ps