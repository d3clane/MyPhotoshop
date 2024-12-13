#include "canvas.hpp"

#include <cassert>
#include <iostream>

using namespace psapi;
using namespace psapi::sfm;

namespace ps
{

// canvas saver action implementation

CanvasSaverAction::CanvasSaverAction(std::unique_ptr<ICanvasSnapshot> pastSnapshot,
                                     std::unique_ptr<ICanvasSnapshot> futureSnapshot)
    : pastSnapshot_(std::move(pastSnapshot)), futureSnapshot_(std::move(futureSnapshot))
{
}

bool CanvasSaverAction::isUndoable(const Key& /* key */)
{
    return true;
}

bool CanvasSaverAction::execute(const Key& /* key */)
{
    return true;
}

bool CanvasSaverAction::undo(const Key& /* key */)
{
    ICanvas* canvas = static_cast<ICanvas*>(getRootWindow()->getWindowById(kCanvasWindowId));
    assert(canvas);

    canvas->restore(pastSnapshot_.get());

    return true;
}

bool CanvasSaverAction::redo(const Key& /* key */)
{
    ICanvas* canvas = static_cast<ICanvas*>(getRootWindow()->getWindowById(kCanvasWindowId));
    assert(canvas);

    canvas->restore(futureSnapshot_.get());

    return true;
}

void CanvasSaverAction::setPastSnapshot(std::unique_ptr<ICanvasSnapshot> snapshot)
{
    pastSnapshot_ = std::move(snapshot);
}

void CanvasSaverAction::setFutureSnapshot(std::unique_ptr<ICanvasSnapshot> snapshot)
{
    futureSnapshot_ = std::move(snapshot);
}

// functions

void copyLayerToLayer(ILayer* dst, const ILayer* src, const vec2u& size)
{
    // TODO: maybe unzoom layer, because need to copy fullSize, not only part of the size? 

    for (size_t x = 0; x < size.x; ++x)
    {
        for (size_t y = 0; y < size.y; ++y)
        {
            Color color = src->getPixel({static_cast<int>(x), static_cast<int>(y)});
            if (color.a == 0)
                continue;

            dst->setPixel({static_cast<int>(x), static_cast<int>(y)}, color);
        }
    }
}

void copyImageToLayer(ILayer* dst, const IImage* src, const vec2i& layerPos)
{
    vec2i beginPos = src->getPos();
    vec2u imageSize = src->getSize();

    for (int x = 0; x < static_cast<int>(imageSize.x); ++x)
    {
        for (int y = 0; y < static_cast<int>(imageSize.y); ++y)
        {
            Color pixel = src->getPixel(static_cast<unsigned>(x), 
                                        static_cast<unsigned>(y));
            if (pixel.a == 0)
                continue;

            dst->setPixel(vec2i{x + beginPos.x - layerPos.x, y + beginPos.y - layerPos.y}, pixel);
        }
    }
}

std::unique_ptr<IImage> copyLayerToImage(const ILayer* src, const vec2u& size)
{
    std::unique_ptr<IImage> image = IImage::create();
    image->create(size, getLayerScreenIn1D(src, size).data());

    return image;
}

std::vector<std::vector<Color>> getLayerScreenIn2D(const ILayer* layer, const vec2u& size)
{
    std::vector<std::vector<Color>> pixels(size.y, std::vector<Color>(size.x));

    for (size_t x = 0; x < size.x; ++x)
        for (size_t y = 0; y < size.y; ++y)
            pixels[y][x] = layer->getPixel(vec2i{static_cast<int>(x), static_cast<int>(y)});

    return pixels;
}

std::vector<Color> getLayerScreenIn1D(const ILayer* layer, const vec2u& size)
{
    std::vector<Color> pixels;

    for (unsigned y = 0; y < size.y; ++y)
        for (unsigned x = 0; x < size.x; ++x)
            pixels.push_back(layer->getPixel(vec2i{static_cast<int>(x), static_cast<int>(y)}));

    return pixels;
}

void copyPixelsToLayer(ILayer* layer, const std::vector<std::vector<Color>>& pixels)
{
    for (size_t y = 0; y < pixels.size(); ++y)
        for (size_t x = 0; x < pixels[y].size(); ++x)
            layer->setPixel(vec2i{static_cast<int>(x), static_cast<int>(y)}, pixels[y][x]);
}

} // namespace ps