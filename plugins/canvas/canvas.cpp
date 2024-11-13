#include "canvas.hpp"
#include "api/api_sfm.hpp"
#include "scrollbar/scrollbar.hpp"

#include <cassert>
#include <iostream>

using namespace ps;

namespace ps
{

namespace 
{

std::unique_ptr<IRectangleShape> createColorRectShape(Color color)
{
    std::unique_ptr<IRectangleShape> shape = IRectangleShape::create(0, 0);

    shape->setFillColor(color);
    shape->setOutlineThickness(0);

    return shape;
}

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
    return static_cast<size_t>((area.pos.y + pos.y) * fullSize.x + area.pos.x + pos.x);
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
            size_t pixelPos = getCutRectPosInFullPixelsArray(area, vec2i{x, y}, pixelsArrayFullSize);
            result[(y * size.x + x)] = pixels[pixelPos];
        }
    }

    return result;
}

} // namespace anonymous

// Layer implementation

Layer::Layer(vec2u size) : fullSize_(size), pixels_(fullSize_.x * fullSize_.y)
{
}

Color Layer::getPixelOnScreen(vec2i pos) const 
{
    if (pos.x < 0 || pos.y < 0 || pos.x >= area_.size.x || pos.y >= area_.size.y)
        return {0, 0, 0, 0};

    return pixels_.at(getCutRectPosInFullPixelsArray(area_, pos, fullSize_));
}

void Layer::setPixelOnScreen(vec2i pos, Color pixel) 
{
    if (pos.x < 0 || pos.y < 0 || pos.x >= area_.size.x || pos.y >= area_.size.y)
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

// Canvas implementation

Canvas::Canvas(vec2i pos, vec2u size) : pos_(pos), size_(size)
{
    fullSize_ = calculateFullSize(size);

    tempLayer_ = std::make_unique<Layer>(fullSize_);
    
    boundariesShape_ = IRectangleShape::create(size_.x, size_.y);

    boundariesShape_->setFillColor({255, 255, 255, 255});
    boundariesShape_->setPosition(pos_);
    boundariesShape_->setOutlineThickness(0);

    layers_.push_back(std::make_unique<Layer>(fullSize_));
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

    vec2u renderWindowSize = renderWindow->getSize();
    setSize({renderWindowSize.x * CanvasSize.x , renderWindowSize.y * CanvasSize.y});
    setPos ({CanvasTopLeftPos.x * renderWindowSize.x, CanvasTopLeftPos.y * renderWindowSize.y});

    lastMousePosRelatively_ = Mouse::getPosition(renderWindow) - pos_;

    isPressed_ = updateIsPressed(event, isPressed_, 
                                 checkIsHovered(lastMousePosRelatively_ + pos_, pos_, size_));

    return true;
}

void Canvas::drawLayer(const Layer& layer, IRenderWindow* renderWindow) 
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
    for (int x = 0; x < fullSize_.x; x++) 
    {
        for (int y = 0; y < fullSize_.y; y++) 
            tempLayer_->setPixelOnScreen({x, y}, pixel);
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

    std::unique_ptr<Layer> newLayer = std::make_unique<Layer>(fullSize_);
    for (int x = 0; x < fullSize_.x; x++) 
    {
        for (int y = 0; y < fullSize_.y; y++) 
        {
            vec2i pos = {x, y};
            newLayer->setPixelOnScreen(pos, layer->getPixelOnScreen(pos));
        }
    }

    layers_.insert(layers_.begin() + index, std::move(newLayer));
    return true;
}

bool Canvas::insertEmptyLayer(size_t index) 
{
    if (index > layers_.size()) 
        return false;

    layers_.insert(layers_.begin() + index, std::make_unique<Layer>(fullSize_));
    return true;
}

void Canvas::setPos(vec2i pos) 
{
    if (pos.x == pos_.x && pos.y == pos_.y)
        return;
    
    pos_ = pos;

    boundariesShape_->setPosition(pos);
}

void Canvas::setSize(vec2i size) 
{
    if (size_.x == size.x && size_.y == size.y)
        return;
    
    size_ = {static_cast<unsigned>(size.x), static_cast<unsigned>(size.y)};
    fullSize_ = calculateFullSize(size_);

    boundariesShape_->setSize({size_.x, size_.y});

    CutRect cutRectangle{calculateCutRectangleTopLeft(fullSize_, size_, scroll_), size_};

    tempLayer_->changeFullSize(fullSize_);
    tempLayer_->changeArea(cutRectangle);

    for (auto& layer : layers_) 
    {
        layer->changeArea(cutRectangle);
        layer->changeFullSize(fullSize_);
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

} // namespace ps

namespace
{

template<typename T>
std::unique_ptr<AScrollBar> createScrollBar(const Canvas* canvas)
{
    std::unique_ptr<AScrollBar> scrollBar{new T{vec2i{0, 0}, vec2u{0, 0}, kInvalidWindowId}};
    scrollBar->setShape(createColorRectShape({0, 0, 0, 255}));
    scrollBar->setParent(static_cast<const ICanvas*>(canvas));

    return scrollBar;
}

template<typename T>
std::unique_ptr<AScrollBarButton> createMoveButton(const AScrollBar* scrollBar, Canvas* canvas)
{
    std::unique_ptr<AScrollBarButton> moveButton{
        new T{vec2i{0, 0}, vec2u{0, 0}, kInvalidWindowId}
    };

    moveButton->setShape(createColorRectShape({211, 211, 211, 120}), PressButton::State::Normal );
    moveButton->setShape(createColorRectShape({169, 169, 169, 120}), PressButton::State::Hovered);
    moveButton->setShape(createColorRectShape({128, 128, 128, 120}), PressButton::State::Pressed);

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

} // namespace anonymous

bool loadPlugin()
{
    auto rootWindow = getRootWindow();

    std::unique_ptr<Canvas> canvas = createCanvas();
    std::unique_ptr<AScrollBar> scrollBarX = createScrollBar<ScrollBarX>(canvas.get());
    std::unique_ptr<AScrollBarButton> moveButtonX = 
        createMoveButton<ScrollBarButtonX>(scrollBarX.get(), canvas.get());

    std::unique_ptr<AScrollBar> scrollBarY = createScrollBar<ScrollBarY>(canvas.get());
    std::unique_ptr<AScrollBarButton> moveButtonY = 
        createMoveButton<ScrollBarButtonY>(scrollBarY.get(), canvas.get());

    scrollBarX->setMoveButton(std::move(moveButtonX));
    scrollBarY->setMoveButton(std::move(moveButtonY));

    std::unique_ptr<ScrollBarsXYManager> scrollBarsXYManager = std::make_unique<ScrollBarsXYManager>(
        std::unique_ptr<ScrollBarX>(static_cast<ScrollBarX*>(scrollBarX.release())), 
        std::unique_ptr<ScrollBarY>(static_cast<ScrollBarY*>(scrollBarY.release())));

    rootWindow->addWindow(std::unique_ptr<ICanvas>(canvas.release()));
    rootWindow->addWindow(std::unique_ptr<IWindowContainer>(scrollBarsXYManager.release()));

#if 0
    rootWindow->addWindow(std::unique_ptr<IWindowContainer>(scrollBarX.release()));
    rootWindow->addWindow(std::unique_ptr<IWindowContainer>(scrollBarY.release()));
#endif

    return true;
}

void unloadPlugin()
{
    getRootWindow()->removeWindow(kCanvasWindowId);
    return;
}
