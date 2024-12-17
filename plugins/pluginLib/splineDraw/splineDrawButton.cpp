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
    if (canvasSaver_.isSavingComplete())
        return canvasSaver_.flushCanvasSaving();
    else 
        return std::make_unique<UpdateCallbackAction<SplineDrawButton>>(*this, renderWindow, event);
}

bool SplineDrawButton::update(const IRenderWindow* renderWindow, const Event& event)
{
    State prevState = state_;
    bool updatedState = updateState(renderWindow, event);

    if (state_ != State::Released)
        return updatedState;
    
    updateOptionsBar(state_, prevState);

    ICanvas* canvas = static_cast<ICanvas*>(getRootWindow()->getWindowById(kCanvasWindowId));
    assert(canvas);

    if (!canvas->isPressedLeftMouseButton())
    {
        if (canvasIsAlreadyPressed_)
            canvasSaver_.canvasSaveEnd();

        interpolator_.clear();

        canvasIsAlreadyPressed_ = false;
        return true;
    }

    if (!canvasIsAlreadyPressed_)
        canvasSaver_.canvasSaveBegin();

    canvasIsAlreadyPressed_ = true;

    if (drawTrace(canvas))
        interpolator_.popFront();

    vec2i mousePos = canvas->getMousePosition();
    interpolator_.pushBack({mousePos.x, mousePos.y});

    return updatedState;
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