#include "canvas.hpp"
#include "api/api_sfm.hpp"
#include "scrollbar.hpp"

#include <cassert>
#include <iostream>

using namespace ps;

namespace ps
{

namespace 
{

std::unique_ptr<IRectangleShape> createColorRectShape(size_t width, size_t height, Color color)
{
    std::unique_ptr<IRectangleShape> shape = IRectangleShape::create(width, height);

    shape->setFillColor(color);
    shape->setOutlineThickness(0);

    return shape;
}

} // namespace anonymous

// Layer implementation

Layer::Layer(vec2u size)
    : size_(size),
      pixels_(static_cast<size_t>(size.x) * static_cast<size_t>(size.y), 
              {0, 0, 0, 0})
{
}

Color Layer::getPixel(vec2i pos) const 
{
    if (pos.x < 0 || pos.y < 0 || pos.x >= size_.x || pos.y >= size_.y)
        return {0, 0, 0, 0};

    return pixels_.at(static_cast<size_t>(pos.y * size_.x + pos.x));
}

void Layer::setPixel(vec2i pos, Color pixel) 
{
    if (pos.x < 0 || pos.y < 0 || pos.x >= size_.x || pos.y >= size_.y)
        return;

    pixels_.at(static_cast<size_t>(pos.y * size_.x + pos.x)) = pixel;
}

void Layer::changeSize(vec2u size) 
{   
    auto newPixels = std::vector<Color>(size.x * size.y);

    for (size_t x = 0; x < size.x; ++x)
    {
        for (size_t y = 0; y < size.y; ++y)
        {
            if (x >= size_.x || y >= size_.y)
                continue;
            
            newPixels[(y * size.x + x)] = getPixel({static_cast<int>(x), static_cast<int>(y)});
        }
    }

    size_ = size;
    pixels_.swap(newPixels);
}

// Canvas implementation

Canvas::Canvas(vec2i pos, vec2u size)
    : tempLayer_(std::make_unique<Layer>(size)),
      AWindow(pos, size, kCanvasWindowId)
{
    boundariesShape_ = createColorRectShape(size_.x, size_.y, {255, 255, 255, 255});

    boundariesShape_->setPosition(pos_);
    boundariesShape_->setOutlineThickness(0);

    layers_.push_back(std::make_unique<Layer>(size_));
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

bool Canvas::update(const IRenderWindow* renderWindow, const Event& event)
{
    if (!isActive_)
        return false;

    auto renderWindowSize = renderWindow->getSize();
    setSize({renderWindowSize.x * CanvasSize.x , renderWindowSize.y * CanvasSize.y});
    setPos ({CanvasTopLeftPos.x * renderWindowSize.x, CanvasTopLeftPos.y * renderWindowSize.y});

    lastMousePosRelatively_ = Mouse::getPosition(renderWindow) - pos_;

    isPressed_ = updateIsPressed(event, isPressed_, lastMousePosRelatively_ + pos_);

    return true;
}

void Canvas::drawLayer(const Layer& layer, IRenderWindow* renderWindow) 
{
    auto texture = ITexture::create();
    texture->create(size_.x, size_.y);

    if (!layer.pixels_.data()) 
        return;

    texture->update(layer.pixels_.data(), size_.x, size_.y, 0, 0);

    auto sprite = ISprite::create();
    sprite->setTexture(texture.get());
    
    sprite->setPosition(pos_.x, pos_.y);

    renderWindow->draw(sprite.get());
}

vec2i Canvas::getMousePosition() const
{
    return { lastMousePosRelatively_.x, lastMousePosRelatively_.y };
}

bool Canvas::isPressed() const
{
    return isPressed_;
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
    for (int x = 0; x < size_.x; x++) 
    {
        for (int y = 0; y < size_.y; y++) 
            tempLayer_->setPixel({x, y}, pixel);
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

    layers_.erase(layers_.begin() + index);
    return true;
}

bool Canvas::insertLayer(size_t index, std::unique_ptr<ILayer> layer) 
{
    if (index > layers_.size()) {
        return false;
    }

    std::unique_ptr<Layer> new_layer = std::make_unique<Layer>(size_);
    for (int x = 0; x < size_.x; x++) 
    {
        for (int y = 0; y < size_.y; y++) 
        {
            vec2i pos = {x, y};
            new_layer->setPixel(pos, layer->getPixel(pos));
        }
    }

    layers_.insert(layers_.begin() + index, std::move(new_layer));
    return true;
}

bool Canvas::insertEmptyLayer(size_t index) 
{
    if (index > layers_.size()) 
        return false;

    layers_.insert(layers_.begin() + index, std::make_unique<Layer>(size_));
    return true;
}

void Canvas::setPos(vec2i pos) 
{
    pos_ = pos;

    boundariesShape_->setPosition(pos);
}

void Canvas::setSize(vec2i size) 
{
    size_ = {static_cast<unsigned>(size.x), static_cast<unsigned>(size.y)};

    boundariesShape_->setSize({size_.x, size_.y});

    tempLayer_->changeSize(size_);
    for (auto& layer : layers_) {
        layer->changeSize(size_);
    }
}

void Canvas::setScale(vec2f scale) 
{
    setSize({static_cast<unsigned>(size_.x * scale.x / scale_.x), 
             static_cast<unsigned>(size_.y * scale.y / scale_.y)});

    scale_ = scale;
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
    assert(false);
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

} // namespace ps

namespace
{

std::unique_ptr<ScrollBar> createScrollBar()
{
    std::unique_ptr<ScrollBar> scrollBar{new ScrollBar{vec2i{175, 100}, vec2u{1000, 50}, kInvalidWindowId}};
    auto scrollBarShape = createColorRectShape(1000, 50, {255, 255, 255, 255});
    scrollBar->setShape(std::move(scrollBarShape));
    scrollBarShape.release();

    return scrollBar;
}

std::unique_ptr<ICanvas> createCanvas()
{
    const vec2i canvasPos = {0, 0};
    const vec2u canvasSize = {0, 0};
    
    return std::unique_ptr<ICanvas>(new Canvas{canvasPos, canvasSize});
}

std::unique_ptr<ScrollBarButton> createMoveButton()
{
    std::unique_ptr<ScrollBarButton> moveButton{
        new ScrollBarButton{vec2i{175, 100}, vec2u{150, 50}, kInvalidWindowId}
    };

    moveButton->setShape(createColorRectShape(150, 50, {211, 211, 211, 255}), PressButton::State::Normal);
    moveButton->setShape(createColorRectShape(150, 50, {169, 169, 169, 255}), PressButton::State::Hovered);
    moveButton->setShape(createColorRectShape(150, 50, {128, 128, 128, 255}), PressButton::State::Pressed);

    return moveButton;
}

} // namespace anonymous

bool loadPlugin()
{
    auto rootWindow = getRootWindow();

    std::unique_ptr<ICanvas> canvas = createCanvas();
    std::unique_ptr<ScrollBar> scrollBar = createScrollBar();
    std::unique_ptr<ScrollBarButton> moveButton = createMoveButton();

    scrollBar->setMoveButton(std::move(moveButton));
    moveButton.release();

    rootWindow->addWindow(std::move(canvas));
    rootWindow->addWindow(std::unique_ptr<IWindowContainer>(scrollBar.release()));
    canvas.release();

    return true;
}

void unloadPlugin()
{
    getRootWindow()->removeWindow(kCanvasWindowId);
    return;
}
