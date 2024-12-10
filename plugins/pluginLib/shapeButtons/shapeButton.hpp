#ifndef PLUGINS_PLUGIN_LIB_SHAPE_BUTTONS_SHAPE_BUTTON_HPP
#define PLUGINS_PLUGIN_LIB_SHAPE_BUTTONS_SHAPE_BUTTON_HPP

#include <string>
#include <cassert>

#include "api/api_sfm.hpp"
#include "api/api_photoshop.hpp"
#include "api/api_bar.hpp"
#include "api/api_canvas.hpp"
#include "pluginLib/canvas/canvas.hpp"

#include "pluginLib/bars/ps_bar.hpp"
#include "pluginLib/windows/windows.hpp"

#include "pluginLib/instrumentBar/mediator.hpp"
#include "pluginLib/instrumentBar/instrumentBar.hpp"

#include "pluginLib/toolbar/toolbarButton.hpp"
#include "pluginLib/actions/actions.hpp"

#include <iostream>

using namespace ps;
using namespace psapi;
using namespace psapi::sfm;

namespace ps
{

namespace
{

using MediatorType = APropertiesMediator;

struct ShapeBounds
{
    vec2i topLeft;
    vec2i bottomRight;
};

ShapeBounds getShapeBounds(const vec2i& topLeftNow, const vec2i& bottomRightNow)
{
    vec2i topLeft = topLeftNow;
    vec2i bottomRight = bottomRightNow;

    if (topLeftNow.x > bottomRightNow.x)
    {
        topLeft.x = bottomRightNow.x;
        bottomRight.x = topLeftNow.x;
    }

    if (topLeftNow.y > bottomRightNow.y)
    {
        topLeft.y = bottomRightNow.y;
        bottomRight.y = topLeftNow.y;
    }

    return {topLeft, bottomRight};
}

template<typename T>
void copyShapeToLayer(ILayer* layer, const T* shape, const vec2i& canvasPos)
{
    const IImage* image = shape->getImage();
    if (!image)
        return;
    
    copyImageToLayer(layer, image, canvasPos);
}

} // namespace anonymous

template<typename T>
class ShapeButton : public AInstrumentButton<MediatorType>
{
public:
    ShapeButton() = default;
    ShapeButton(std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture);

    std::unique_ptr<IAction> createAction(const IRenderWindow* renderWindow, const Event& event) override;
    bool update(const IRenderWindow* renderWindow, const Event& event);
    void draw(IRenderWindow* renderWindow) override;

private:
    bool canvasIsAlreadyPressed_ = false;
    vec2i beginShapePos_;
};

template<typename T>
std::unique_ptr<T> createShape(const vec2i& beginShapePos, const ICanvas* canvas,
                               std::shared_ptr<MediatorType> mediator)
{
    vec2i canvasPos  = canvas->getPos();
    vec2i mousePos = canvas->getMousePosition() + canvasPos;

    ShapeBounds bounds = getShapeBounds(beginShapePos, mousePos);

    vec2i topLeft     = bounds.topLeft;
    vec2i bottomRight = bounds.bottomRight;
    
    assert(bottomRight.y >= topLeft.y);
    assert(bottomRight.x >= topLeft.x);
    const unsigned shapeYSize = static_cast<unsigned>(bottomRight.y - topLeft.y);
    const unsigned shapeXSize = static_cast<unsigned>(bottomRight.x - topLeft.x);

    std::unique_ptr<T> shape = T::create(shapeXSize, shapeYSize);
    assert(shape);

    DrawingProperties fillProperties = mediator->fillProperties();
    Color color = fillProperties.color;

    shape->setFillColor(color);
    shape->setOutlineThickness(0);
    shape->setPosition(topLeft);

    return shape;
}

template<typename T>
ShapeButton<T>::ShapeButton(std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture)
{
    mainSprite_ = std::move(sprite);
    mainTexture_ = std::move(texture);
}

template<typename T>
std::unique_ptr<IAction> ShapeButton<T>::createAction(const IRenderWindow* renderWindow, 
                                                      const Event& event)
{
    return std::make_unique<UpdateCallbackAction<ShapeButton>>(*this, renderWindow, event);
}

template<typename T>
bool ShapeButton<T>::update(const IRenderWindow* renderWindow, const Event& event)
{
    bool updateStateRes = updateState(renderWindow, event);

#if 0
    instrument_button_functions::updateInstrumentBar(instrumentBar_.get(), state_,
                                                    renderWindow, event);
#endif

    if (state_ != State::Released)
        return updateStateRes;

    ICanvas* canvas = static_cast<ICanvas*>(getRootWindow()->getWindowById(kCanvasWindowId));
    if (!canvas)
    {
        std::cerr << "CANVAS NOT FOUND!\n";
        assert(0);
    }
    
    size_t activeLayerIndex = canvas->getActiveLayerIndex();
    ILayer* activeLayer = canvas->getLayer(activeLayerIndex);
    ILayer* tempLayer = canvas->getTempLayer();

    vec2i canvasPos = canvas->getPos();

    if (!canvas->isPressedLeftMouseButton() && canvasIsAlreadyPressed_)
    {
        copyShapeToLayer(activeLayer, 
                         createShape<T>(beginShapePos_, canvas, mediator_).get(), 
                         canvasPos);
        tempLayer->removeAllDrawables();
    }
    
    if (!canvas->isPressedLeftMouseButton())
    {
        canvasIsAlreadyPressed_ = false;
        return updateStateRes;
    }

    if (!canvasIsAlreadyPressed_)
    {
        beginShapePos_ = canvas->getMousePosition() + canvasPos;
        canvasIsAlreadyPressed_ = true;
    }

    std::unique_ptr<T> shape = createShape<T>(beginShapePos_, canvas, mediator_);
    tempLayer->removeAllDrawables();
    tempLayer->addDrawable(std::move(shape));

    return true;
}

template<typename T>
void ShapeButton<T>::draw(IRenderWindow* renderWindow)
{
    ASpritedBarButton::draw(renderWindow, parent_);

#if 0
    instrument_button_functions::drawInstrumentBar(instrumentBar_.get(), renderWindow);
#endif    
}

} // namespace ps

#endif // PLUGINS_PLUGIN_LIB_SHAPE_BUTTONS_SHAPE_BUTTON_HPP