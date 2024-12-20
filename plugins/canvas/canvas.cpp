#include "canvas.hpp"
#include "api/api_sfm.hpp"
#include "pluginLib/scrollbar/scrollbar.hpp"
#include "plugins/pluginLib/actions/actions.hpp" // TODO: ?
#include "interfaceInfo/interfaceInfo.hpp"

#include <cassert>
#include <iostream>

using namespace ps;

namespace ps
{

namespace 
{

vec2u calculateFullSize(vec2u size)
{
    static const size_t prettyCoeff = 2;

    return size * prettyCoeff;
}

vec2i calculateCutRectangleTopLeft(vec2u fullSize, vec2u visibleSize, vec2f scroll)
{
    return vec2i(static_cast<int>(scroll.x * static_cast<float>(fullSize.x - visibleSize.x)), 
                 static_cast<int>(scroll.y * static_cast<float>(fullSize.y - visibleSize.y)));
}

size_t getCutRectPosInFullPixelsArray(CutRect area, vec2i pos, vec2u fullSize)
{
    return static_cast<size_t>((area.pos.y + pos.y) * static_cast<int>(fullSize.x) + area.pos.x + pos.x);
}

std::vector<Color> cutRectangle(const std::vector<Color>& pixels, vec2u pixelsArrayFullSize, 
                                vec2i pos, vec2u size)
{
    std::vector<Color> result(size.x * size.y);

    CutRect area = {pos, size};

    for (size_t x = 0; x < size.x; ++x)
    {
        for (size_t y = 0; y < size.y; ++y)
        {
            size_t pixelPos = getCutRectPosInFullPixelsArray(area, 
                                                             vec2i{static_cast<int>(x), static_cast<int>(y)}, 
                                                             pixelsArrayFullSize);
            result[(y * size.x + x)] = pixels[pixelPos];
        }
    }

    return result;
}

} // namespace anonymous

// Layer snapshot implementation

LayerSnapshot::LayerSnapshot(const std::vector<std::shared_ptr<Drawable>>& drawables,
                             const std::vector<Color>& pixels) 
 : drawables_(drawables), pixels_(pixels) 
{
}

std::vector<std::shared_ptr<Drawable>> LayerSnapshot::getDrawables() const { return drawables_; }
std::vector<Color> LayerSnapshot::getPixels() const { return pixels_; }

// Layer implementation

Layer::Layer(vec2u size, vec2u fullSize) 
    : size_(size), fullSize_(fullSize),
      pixels_(fullSize_.x * fullSize_.y, Color{}.getStandardColor(Color::Type::White))
{
}

drawable_id_t Layer::addDrawable(std::unique_ptr<Drawable> object)
{
    // TODO: pretty bad logic, cleaning vector only on removeAllDrawables.

    drawables_.push_back(std::move(object));
    return static_cast<drawable_id_t>(drawables_.size()) - 1;
}

void Layer::removeDrawable(drawable_id_t id)
{
    assert(static_cast<size_t>(id) < drawables_.size());
    assert(id >= 0);
    
    drawables_[static_cast<size_t>(id)].reset();
}

void Layer::removeAllDrawables()
{
    drawables_.clear();
}

vec2u Layer::getSize() const
{
    return size_;
}

Color Layer::getPixel(vec2i pos) const 
{
    if (pos.x < 0 || pos.y < 0 || 
        pos.x >= static_cast<int>(area_.size.x) || pos.y >= static_cast<int>(area_.size.y))
        return {0, 0, 0, 0};

    return pixels_.at(getCutRectPosInFullPixelsArray(area_, pos, fullSize_));
}

void Layer::setPixel(vec2i pos, Color pixel) 
{
    if (pos.x < 0 || pos.y < 0 || 
        pos.x >= static_cast<int>(area_.size.x) || pos.y >= static_cast<int>(area_.size.y))
        return;

    size_t pixelPos = getCutRectPosInFullPixelsArray(area_, pos, fullSize_);
    pixels_.at(pixelPos) = pixel;
}

void Layer::changeFullSize(vec2u size) 
{   
    std::vector<Color> newPixels(size.x * size.y);

    for (size_t x = 0; x < size.x; ++x)
    {
        for (size_t y = 0; y < size.y; ++y)
        {
            if (x >= fullSize_.x || y >= fullSize_.y)
                continue;
            
            newPixels[(y * size.x + x)] = pixels_[y * fullSize_.x + x];
        }
    }

    fullSize_ = size;
    pixels_.swap(newPixels);
}

void Layer::changeArea(const CutRect& area)
{
    area_ = area;
}

std::unique_ptr<ILayerSnapshot> Layer::save() // NOTE: const))
{
    return std::make_unique<LayerSnapshot>(drawables_, pixels_);
}

void Layer::restore(ILayerSnapshot* snapshot)
{
    auto layerSnapshot = dynamic_cast<LayerSnapshot*>(snapshot);
    assert(layerSnapshot);

    drawables_ = layerSnapshot->getDrawables();
    pixels_ = layerSnapshot->getPixels();
}

// Canvas snapshot implementation

CanvasSnapshot::CanvasSnapshot(std::unique_ptr<LayerSnapshot> tempLayer, 
                               std::vector<std::unique_ptr<LayerSnapshot>>&& layers) 
 : tempLayer_(std::move(tempLayer))
{
    layers_.swap(layers);
}

LayerSnapshot* CanvasSnapshot::getTempLayerSnapshot() const { return tempLayer_.get(); }

std::vector<LayerSnapshot*> CanvasSnapshot::getLayersSnapshots() const
{
    std::vector<LayerSnapshot*> result;
    for (const auto& layer : layers_)
    {
        assert(layer.get());
        result.push_back(layer.get());
    }
    return result;
}

// Canvas implementation

Canvas::Canvas(vec2i pos, vec2u size) : size_(size), pos_(pos)
{
    fullSize_ = calculateFullSize(size);

    tempLayer_ = std::make_unique<Layer>(size, fullSize_);
    
    boundariesShape_ = IRectangleShape::create(size_.x, size_.y);

    boundariesShape_->setFillColor({255, 255, 255, 255});
    boundariesShape_->setPosition(pos_);
    boundariesShape_->setOutlineThickness(0);

    layers_.push_back(std::make_unique<Layer>(size, fullSize_));
}

void Canvas::draw(IRenderWindow* renderWindow) 
{
    if (!isActive_)
        return;

    renderWindow->draw(boundariesShape_.get());

    for (const auto& layer : layers_) 
    {
        assert(layer.get());

        drawLayer(*layer.get(), renderWindow);
    }
    
    drawLayer(*tempLayer_.get(), renderWindow);

}

std::unique_ptr<IAction> Canvas::createAction(const IRenderWindow* renderWindow, 
                                              const Event& event)
{
    return std::make_unique<UpdateCallbackAction<Canvas>>(*this, renderWindow, event);
}

uint8_t Canvas::updatePressType(uint8_t pressType, const Event& event)
{
    assert(event.type == Event::MouseButtonPressed);

    switch(event.mouseButton.button)
    {
        case Mouse::Button::Left:
            return pressType | static_cast<uint8_t>(PressType::LMB);
        case Mouse::Button::Right:
            return pressType | static_cast<uint8_t>(PressType::RMB);
        case Mouse::Button::Middle:
            return pressType | static_cast<uint8_t>(PressType::MMB);

        case Mouse::Button::XButton1:
        case Mouse::Button::XButton2:
        default:
            return pressType;
    }

    return pressType;
}

bool Canvas::update(const IRenderWindow* renderWindow, const Event& event)
{
    if (!isActive_)
        return false;

    IntRect canvasIntRect = getCanvasIntRect();
    setSize(canvasIntRect.size);
    setPos (canvasIntRect.pos );

    lastMousePosRelatively_ = Mouse::getPosition(renderWindow) - pos_;

    bool isPressed = (pressType_ != 0);

    isPressed = updateIsPressed(event, isPressed, 
                                checkIsHovered(lastMousePosRelatively_, vec2i{0, 0}, size_));

    if (!isPressed)
        pressType_ = 0;
    else if (event.type == Event::MouseButtonPressed)
        pressType_ = updatePressType(pressType_, event);

    return true;
}

void Canvas::drawPixels(const Layer& layer, IRenderWindow* renderWindow)
{
    auto texture = ITexture::create();
    texture->create(size_.x, size_.y);

    if (!layer.pixels_.data())
        return;

    vec2i topLeft = calculateCutRectangleTopLeft(fullSize_, size_, scroll_);
    std::vector<Color> pixels = cutRectangle(layer.pixels_, fullSize_, topLeft, size_);
    texture->update(pixels.data(), size_.x, size_.y, 0, 0);

    auto sprite = ISprite::create();
    sprite->setTexture(texture.get());
    sprite->setPosition(pos_.x, pos_.y);

    renderWindow->draw(sprite.get());
}

void Canvas::drawDrawables(const Layer& layer, IRenderWindow* renderWindow)
{
    for (const auto& drawable : layer.drawables_) 
    {
        assert(drawable.get());
        drawable->draw(renderWindow);
    }
}

void Canvas::drawLayer(const Layer& layer, IRenderWindow* renderWindow) 
{
    drawPixels(layer, renderWindow);
    drawDrawables(layer, renderWindow);
}

vec2i Canvas::getMousePosition() const
{
    return { lastMousePosRelatively_.x, lastMousePosRelatively_.y };
}

bool Canvas::isPressedLeftMouseButton() const
{
    return pressType_ & static_cast<uint8_t>(PressType::LMB);
}

bool Canvas::isPressedRightMouseButton() const
{
    return pressType_ & static_cast<uint8_t>(PressType::RMB);
}

bool Canvas::isPressedScrollButton() const
{
    return pressType_ & static_cast<uint8_t>(PressType::MMB);
}

ILayer* Canvas::getLayer(size_t index)
{
    return layers_.at(index).get();
}

const ILayer* Canvas::getLayer(size_t index) const 
{
    return layers_.at(index).get();
}

ILayer* Canvas::getTempLayer() 
{
    return tempLayer_.get();
}

const ILayer* Canvas::getTempLayer() const 
{
    return tempLayer_.get();
}

void Canvas::cleanTempLayer() 
{
    Color pixel = {0u, 0u, 0u, 0u}; // important that alpha is 0
    for (int x = 0; x < static_cast<int>(fullSize_.x); x++) 
    {
        for (int y = 0; y < static_cast<int>(fullSize_.y); y++) 
            tempLayer_->setPixel(vec2i{x, y}, pixel);
    }
}

size_t Canvas::getNumLayers() const 
{
    return layers_.size();
}

bool Canvas::removeLayer(size_t index) 
{
    if (index >= layers_.size()) 
        return false;

    layers_.erase(layers_.begin() + static_cast<long>(index));
    return true;
}

bool Canvas::insertLayer(size_t index, std::unique_ptr<ILayer> layer) 
{
    if (index > layers_.size()) {
        return false;
    }

    std::unique_ptr<Layer> newLayer = std::make_unique<Layer>(size_, fullSize_);
    for (int x = 0; x < static_cast<int>(fullSize_.x); x++) 
    {
        for (int y = 0; y < static_cast<int>(fullSize_.y); y++) 
        {
            vec2i pos = {x, y};
            newLayer->setPixel(pos, layer->getPixel(pos));
        }
    }

    layers_.insert(layers_.begin() + static_cast<long>(index), std::move(newLayer));
    return true;
}

bool Canvas::insertEmptyLayer(size_t index) 
{
    if (index > layers_.size()) 
        return false;

    layers_.insert(layers_.begin() + static_cast<long>(index), 
                   std::make_unique<Layer>(size_, fullSize_));
    return true;
}

void Canvas::setPos(const vec2i& pos) 
{
    if (pos.x == pos_.x && pos.y == pos_.y)
        return;
    
    pos_ = pos;

    boundariesShape_->setPosition(pos);
}

void Canvas::setSize(const vec2u& size) 
{
    if (size_.x == size.x && size_.y == size.y)
        return;
    
    size_ = size;
    fullSize_ = calculateFullSize(size_);

    boundariesShape_->setSize({size_.x, size_.y});

    CutRect cutRectangle{calculateCutRectangleTopLeft(fullSize_, size_, scroll_), size_};

    tempLayer_->changeFullSize(fullSize_);
    tempLayer_->changeArea(cutRectangle);

    for (auto& layer : layers_) 
    {
        layer->changeArea(cutRectangle);
        layer->changeFullSize(fullSize_);
        layer->size_ = size;
    }
}

void Canvas::setZoom(vec2f zoom)
{
    zoom_ = zoom;
    assert(false);
    // TODO: 
}

size_t Canvas::getActiveLayerIndex() const 
{
    return activeLayer_;
}

void Canvas::setActiveLayerIndex(size_t index) 
{
    if (index >= layers_.size())
        return;

    activeLayer_ = index;
}

bool Canvas::isActive() const
{
    return isActive_;
}

wid_t Canvas::getId() const
{
    return kCanvasWindowId;
}

IWindow* Canvas::getWindowById(wid_t id)
{
    return const_cast<IWindow*>(const_cast<const Canvas*>(this)->getWindowById(id));
}

const IWindow* Canvas::getWindowById(wid_t id) const
{
    if (id == kCanvasWindowId)
        return static_cast<const ICanvas*>(this);
    
    return nullptr;
}

vec2i Canvas::getPos() const
{
    return pos_;
}

vec2u Canvas::getSize() const
{
    return size_;
}

void Canvas::setParent(const IWindow* parent)
{
    parent_ = parent;
}

void Canvas::forceActivate() 
{
    isActive_ = true;
}

void Canvas::forceDeactivate()
{
    isActive_ = false;
}

bool Canvas::isWindowContainer() const
{
    return false;
}

void Canvas::scroll(vec2f delta)
{
    setScroll(scroll_ + delta);
}

void Canvas::setScroll(vec2f scroll)
{
    vec2f newScroll = scroll;

    newScroll.x = newScroll.x > 1 ? 1.f : newScroll.x;
    newScroll.y = newScroll.y > 1 ? 1.f : newScroll.y;
    newScroll.x = newScroll.x < 0 ? 0.f : newScroll.x;
    newScroll.y = newScroll.y < 0 ? 0.f : newScroll.y;

    scroll_ = newScroll;

    CutRect cutRectangle{calculateCutRectangleTopLeft(fullSize_, size_, scroll_), size_};

    for (auto& layer : layers_) 
        layer->changeArea(cutRectangle);

    tempLayer_->changeArea(cutRectangle);
}

vec2f Canvas::getScroll()
{
    return scroll_;
}

vec2u Canvas::getVisibleSize()
{
    return size_;
}

vec2u Canvas::getFullSize()
{
    return fullSize_;
}

Color Canvas::getCanvasBaseColor() const
{
    Color color = Color::getStandardColor(Color::Type::White);
    color.a = 0;
    return color;
}

std::unique_ptr<ICanvasSnapshot> Canvas::save()
{
    std::vector<std::unique_ptr<LayerSnapshot>> layersSnapshots;

    for (auto& layer : layers_)
    {
        std::unique_ptr<ILayerSnapshot> iLayerSnapshot = layer->save();
        std::unique_ptr<LayerSnapshot> layerSnapshot{
            static_cast<LayerSnapshot*>(iLayerSnapshot.release())
        };
        
        layersSnapshots.push_back(std::move(layerSnapshot));
    }

    std::unique_ptr<ILayerSnapshot> iTempLayerSnapshot = tempLayer_->save();
    std::unique_ptr<LayerSnapshot> tempLayerSnapshot{
        static_cast<LayerSnapshot*>(iTempLayerSnapshot.release())
    };
    
    return std::make_unique<CanvasSnapshot>(std::move(tempLayerSnapshot), std::move(layersSnapshots));
}

void Canvas::restore(ICanvasSnapshot* snapshot)
{
    auto canvasSnapshot = dynamic_cast<CanvasSnapshot*>(snapshot);
    assert(canvasSnapshot);

    tempLayer_->restore(canvasSnapshot->getTempLayerSnapshot());
    
    std::vector<LayerSnapshot*> layerSnapshots = canvasSnapshot->getLayersSnapshots();

    size_t minSize = std::min(layers_.size(), layerSnapshots.size());
    for (size_t i = 0; i < minSize; ++i)
    {
        layers_[i]->restore(layerSnapshots[i]);
    }

    if (minSize < layers_.size())
        layers_.erase(layers_.begin() + static_cast<ptrdiff_t>(minSize), layers_.end());
    else
    {
        for (size_t i = minSize; i < layerSnapshots.size(); ++i)
        {
            layers_.push_back(std::make_unique<Layer>(size_, fullSize_));
            layers_.back()->restore(layerSnapshots[i]);
        }
    }
}

} // namespace ps

namespace
{

template<typename T>
std::unique_ptr<AScrollBar> createScrollBar(const Canvas* canvas, const char* spriteName, vec2i pos,
                                            vec2i deltaFromPos, vec2u deltaFromSize)
{
    SpriteInfo sprite = createSprite(spriteName);
    vec2u size = sprite.sprite->getSize();

    fprintf(stderr, "POS - %d %d\n", pos.x, pos.y);
    std::unique_ptr<AScrollBar> scrollBar{new T{pos, size, deltaFromPos, deltaFromSize, kInvalidWindowId}};
    scrollBar->setSprite(std::move(sprite));

    scrollBar->setParent(static_cast<const ICanvas*>(canvas));

    return scrollBar;
}

template<typename T>
std::unique_ptr<AScrollBarButton> createMoveButton(const AScrollBar* scrollBar, Canvas* canvas,
                                                   const char* spriteNormal, const char* spriteHover,
                                                   const char* spritePress, const char* leftBoundary,
                                                   const char* rightBoundary)
{
    std::unique_ptr<AScrollBarButton> moveButton{
        new T{vec2i{1, 1}, vec2u{1, 1}, kInvalidWindowId}
    };

    moveButton->setInsideSprite(createSprite(spriteNormal), PressButton::State::Normal);
    moveButton->setInsideSprite(createSprite(spriteHover), PressButton::State::Hovered);
    moveButton->setInsideSprite(createSprite(spritePress), PressButton::State::Pressed);

    moveButton->setLeftBoundarySprite(createSprite(leftBoundary));
    moveButton->setRightBoundarySprite(createSprite(rightBoundary));

    moveButton->setScrollable(canvas);
    moveButton->setParent(static_cast<const IWindowContainer*>(scrollBar));

    return moveButton;
}

std::unique_ptr<Canvas> createCanvas()
{
    const vec2i canvasPos  = {0, 0};
    const vec2u canvasSize = {0, 0};
    
    return std::make_unique<Canvas>(canvasPos, canvasSize);
}

// Can scroll action

class CanScrollAction : public ICanScrollAction
{
public:
    CanScrollAction(ICanvas* canvas) : canvas_(canvas) {}

    bool canScroll(const IRenderWindow* renderWindow, const Event& event) override;

private:
    ICanvas* canvas_;
};

bool CanScrollAction::canScroll(const IRenderWindow* /* renderWindow */, const Event& event)
{
    return ps::checkIsHovered(vec2i{event.mouseWheel.x, event.mouseWheel.y}, 
                              canvas_->getPos(), canvas_->getSize());
}

} // namespace anonymous

bool onLoadPlugin()
{
    auto rootWindow = getRootWindow();

    std::unique_ptr<Canvas> canvas = createCanvas();
    

    std::unique_ptr<AScrollBar> scrollBarX = createScrollBar<ScrollBarX>(
        canvas.get(), "media/textures/scrollX.png", getCanvasDownScrollBarIntRect().pos,
        vec2i{14, 1}, vec2u{28, 2});

    std::unique_ptr<AScrollBarButton> moveButtonX = 
        createMoveButton<ScrollBarButtonX>(scrollBarX.get(), canvas.get(), 
                                           "media/textures/scrollXNormal.png", "media/textures/scrollXHover.png", "media/textures/scrollXPress.png",
                                           "media/textures/scrollXLeftBoundary.png", "media/textures/scrollXRightBoundary.png");

    std::unique_ptr<AScrollBar> scrollBarY = createScrollBar<ScrollBarY>(canvas.get(),
        "media/textures/scrollY.png", getCanvasRightScrollBarIntRect().pos,
        vec2i{1, 14}, vec2u{2, 28});
    
    std::unique_ptr<AScrollBarButton> moveButtonY = 
        createMoveButton<ScrollBarButtonY>(scrollBarY.get(), canvas.get(), 
                                           "media/textures/scrollYNormal.png", "media/textures/scrollYHover.png", "media/textures/scrollYPress.png",
                                           "media/textures/scrollYLeftBoundary.png", "media/textures/scrollYRightBoundary.png");

    scrollBarX->setMoveButton(std::move(moveButtonX));
    scrollBarY->setMoveButton(std::move(moveButtonY));
    
    std::unique_ptr<ScrollBarsXYManager> scrollBarsXYManager = std::make_unique<ScrollBarsXYManager>(
        std::unique_ptr<ScrollBarX>(static_cast<ScrollBarX*>(scrollBarX.release())),
        std::unique_ptr<ScrollBarY>(static_cast<ScrollBarY*>(scrollBarY.release())),
        std::make_unique<CanScrollAction>(canvas.get())
        );

    rootWindow->addWindow(std::unique_ptr<ICanvas>(canvas.release()));
    rootWindow->addWindow(std::unique_ptr<IWindowContainer>(scrollBarsXYManager.release()));
    return true;
}

void onUnloadPlugin()
{
    getRootWindow()->removeWindow(kCanvasWindowId);
    return;
}
