#include "line.hpp"
#include <string>
#include <cassert>

#include "api/api_sfm.hpp"
#include "api/api_photoshop.hpp"
#include "api/api_bar.hpp"
#include "api/api_canvas.hpp"

#include "pluginLib/canvas/canvas.hpp"
#include "pluginLib/bars/ps_bar.hpp"
#include "pluginLib/windows/windows.hpp"
#include "pluginLib/actions/actions.hpp"

#include "pluginLib/toolbar/toolbarButton.hpp"

#include <iostream>

namespace 
{

using namespace ps;
using namespace psapi;
using namespace psapi::sfm;

class LineButton : public AInstrumentButton
{
public:
    LineButton() = default;
    LineButton(std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture);

    std::unique_ptr<IAction> createAction(const IRenderWindow* renderWindow, const Event& event) override;

    bool update(const IRenderWindow* renderWindow, const Event& event);
    void draw(IRenderWindow* renderWindow) override;

private:
    std::unique_ptr<IRectangleShape> createLineShape(vec2i beginPos, const ICanvas* canvas);

private:
    bool canvasIsAlreadyPressed_ = false;

    vec2i lineBeginPos_;
};

void copyLineToLayer(ILayer* layer, const IRectangleShape* line, vec2i canvasPos)
{
    const IImage* image = line->getImage();
    if (!image)
        return;

    copyImageToLayer(layer, image, canvasPos);
}

std::unique_ptr<IRectangleShape> LineButton::createLineShape(vec2i beginPos, const ICanvas* canvas)
{
    vec2i mousePos = canvas->getMousePosition() + canvas->getPos();

    const float lineLength = static_cast<float>(len(beginPos, mousePos));
    const float angle = static_cast<float>(std::atan2(mousePos.y - beginPos.y, mousePos.x - beginPos.x));

    assert(thicknessOption_);
    float thickness = thicknessOption_->getThickness();
    std::unique_ptr<IRectangleShape> line = IRectangleShape::create(static_cast<unsigned>(lineLength), 
                                                                    static_cast<unsigned>(thickness));

    Color color = colorPalette_->getColor();
    line->setFillColor(color);
    line->setOutlineThickness(0);
    line->setRotation(angle * 180.f / static_cast<float>(M_PI));
    line->setPosition(beginPos);

    return line;
}

LineButton::LineButton(std::unique_ptr<ISprite> sprite, std::unique_ptr<ITexture> texture)
{
    mainSprite_ = std::move(sprite);
    mainTexture_ = std::move(texture);
}

std::unique_ptr<IAction> LineButton::createAction(const IRenderWindow* renderWindow, const Event& event)
{
    if (canvasSaver_.isSavingComplete())
        return canvasSaver_.flushCanvasSaving();
    else
        return std::make_unique<UpdateCallbackAction<LineButton>>(*this, renderWindow, event);
}

bool LineButton::update(const IRenderWindow* renderWindow, const Event& event)
{
    State prevState_ = state_;
    bool updateStateRes = updateState(renderWindow, event);
    
    if (state_ != State::Released)
        return updateStateRes;

    updateOptionsBar(state_, prevState_);
    
    ICanvas* canvas = static_cast<ICanvas*>(getRootWindow()->getWindowById(kCanvasWindowId));
    assert(canvas);
    
    size_t activeLayerIndex = canvas->getActiveLayerIndex();
    ILayer* activeLayer = canvas->getLayer(activeLayerIndex);
    ILayer* tempLayer = canvas->getTempLayer();
    vec2i canvasPos = canvas->getPos();

    if (!canvas->isPressedLeftMouseButton() && !canvasIsAlreadyPressed_)
        return updateStateRes;

    if (!canvas->isPressedLeftMouseButton() && canvasIsAlreadyPressed_)
    {
        copyLineToLayer(activeLayer, createLineShape(lineBeginPos_, canvas).get(), canvasPos);
        tempLayer->removeAllDrawables();
        canvasSaver_.canvasSaveEnd();
        canvasIsAlreadyPressed_ = false;
        
        return updateStateRes;
    }
    
    if (!canvasIsAlreadyPressed_)
    {
        lineBeginPos_ = canvas->getMousePosition() + canvasPos;
        canvasIsAlreadyPressed_ = true;
        canvasSaver_.canvasSaveBegin();
    }

    std::unique_ptr<IRectangleShape> line = createLineShape(lineBeginPos_, canvas);
    tempLayer->removeAllDrawables();
    tempLayer->addDrawable(std::move(line));
    
    return true;
}

void LineButton::draw(IRenderWindow* renderWindow)
{
    ASpritedBarButton::draw(renderWindow);
}

} // namespace anonymous

bool onLoadPlugin()
{
    return instrument_button_functions::instrumentButtonOnLoadPlugin<LineButton>(
        "assets/textures/line.png");
}

void onUnloadPlugin()
{
    return;
}
