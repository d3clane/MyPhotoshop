#include "canvas.hpp"
#include "api/api_sfm.hpp"

#include <cassert>
#include <iostream>

using namespace ps;

// Layer implementation

static const char* canvasTextureInputFile = "media/textures/canvas.png";

Layer::Layer(vec2u size)
    : size_(size),
      pixels_(static_cast<size_t>(size.x) *
              static_cast<size_t>(size.y), {0, 0, 0, 0})
{
}

Color Layer::getPixel(vec2i pos) const 
{
    return pixels_.at(static_cast<size_t>(pos.y * size_.x + pos.x));
}

void Layer::setPixel(vec2i pos, Color pixel) 
{
    pixels_.at(static_cast<size_t>(pos.y * size_.x + pos.x)) = pixel;
}

// Canvas implementation

Canvas::Canvas(vec2i pos, vec2u size)
    : tempLayer_(std::make_unique<Layer>(size)),
      size_(size),
      pos_(pos)
{
    boundariesTexture_ = ITexture::create();
    boundariesTexture_->loadFromFile(canvasTextureInputFile);

    boundariesSprite_ = ISprite::create();
    boundariesSprite_->setTexture(boundariesTexture_.get());

    auto spriteSize = boundariesSprite_->getSize();
    //std::cerr << "sprite size - " << boundariesSprite_.getSize().x << " " << boundariesSprite_.getSize().y << "\n";
    boundariesSprite_->setScale(static_cast<double>(size_.x) / spriteSize.x, static_cast<double>(size_.y) / spriteSize.y);
    //std::cerr << "sprite size - " << boundariesSprite_.getSize().x << " " << boundariesSprite_.getSize().y << "\n";
    //pos_ = {200, 200}; // TODO: hardcoded

    boundariesSprite_->setPosition(pos_.x, pos_.y);
    layers_.push_back(std::make_unique<Layer>(size_));
}

void Canvas::draw(IRenderWindow* renderWindow) 
{
    renderWindow->draw(boundariesSprite_.get());

    drawLayer(*layers_[0].get(), renderWindow);
    for (const auto& layer : layers_) 
    {
        assert(layer.get());

        drawLayer(*layer.get(), renderWindow);
    }
    
    drawLayer(*tempLayer_, renderWindow); // TODO: TEMP_LAYER ALWAYS 0 ALPHA WHEN CLEARING PLS PLS PLS
}

bool Canvas::update(const IRenderWindow* renderWindow, const Event& event)
{
    lastMousePosRelatively_ = Mouse::getPosition(renderWindow) - pos_;

    return true;
}

void Canvas::drawLayer(const Layer& layer, IRenderWindow* renderWindow) 
{
#if 0
    int cnt = 0;
    for (size_t i = 0; i < layer.size_.x; ++i)
    {
        for (size_t j = 0; j < layer.size_.y; ++j)
        {
            if (layer.pixels_.data()[i * layer.size_.x + j].a == 255)
                cnt++;
        }
    }
#endif

    auto texture = ITexture::create();
    texture->create(size_.x, size_.y);
    texture->update(layer.pixels_.data(), size_.x, size_.y, 0, 0);
    
    auto sprite = ISprite::create();
    sprite->setTexture(texture.get());
    
    //std::cerr << "sprite size - " << sprite_.getSize().x << " " << sprite_.getSize().y << "\n";

    sprite->setPosition(pos_.x, pos_.y);
    //std::cerr << "sprite pos - " << sprite_.getPosition().x << " " << sprite_.getPosition().y << "\n";

    renderWindow->draw(sprite.get());
}

vec2i Canvas::getMousePosition() const
{
    return { lastMousePosRelatively_.x, lastMousePosRelatively_.y };
}

bool Canvas::isPressed() const
{
    return lastMousePosRelatively_.x >= 0 && lastMousePosRelatively_.y >= 0 &&
           lastMousePosRelatively_.x < size_.x && lastMousePosRelatively_.y < size_.y;
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
}

void Canvas::setSize(vec2i size) 
{
    size_ = {static_cast<unsigned>(size.x), static_cast<unsigned>(size.y)};

    std::cerr << "NO CORRECT IMPLEMENTATION RIGHT NOW\n";
    assert(false);

#if 0
    tempLayer_->changeSize(size_);
    for (auto& layer : layers_) {
        layer->changeSize(size_);
    }
#endif
}

void Canvas::setScale(vec2f scale) 
{
    std::cerr << "NO CORRECT IMPLEMENTATION RIGHT NOW\n";
    assert(false);
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
    std::cerr << "CANVAS IS ACTIVE HAS NO CORRECT IMPL, ALWAYS RETURN TRUE";
    assert(false);

    return true;
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
        return this;
    
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
    std::cerr << "NO PARENT FOR CANVAS\n";
    assert(false);
}

void Canvas::forceActivate() 
{
    std::cerr << "CANVAS ALWAYS ACTIVATED\n";
    assert(false);
}

void Canvas::forceDeactivate()
{
    std::cerr << "CANVAS CAN'T BE DEACTIVATED\n";
    assert(false);
}

bool Canvas::isWindowContainer() const
{
    return false;
}

bool loadPlugin2()
{
    auto rootWindow = getRootWindow();

    const vec2i canvasPos = {200, 200};
    const vec2u canvasSize = {300, 300};
    
    std::unique_ptr<IWindow> canvas{static_cast<IWindow*>(new Canvas{canvasPos, canvasSize})};

    rootWindow->addWindow(std::move(canvas));

    return true;
}

void unloadPlugin2()
{
    getRootWindow()->removeWindow(kCanvasWindowId);
    return;
}
