#ifndef API_CANVAS_HPP
#define API_CANVAS_HPP

#include "api_photoshop.hpp"
#include "api_sfm.hpp"

#include <cstdint>
#include <memory>

namespace psapi {

const wid_t kCanvasWindowId = 100;

class ILayer {
public:
    virtual ~ILayer() = default;
    virtual sfm::Color getPixelOnScreen(sfm::vec2i pos) const = 0;
    virtual void       setPixel(sfm::vec2i pos, sfm::Color pixel) = 0;
};

class ICanvas : public IWindow {
public:
    virtual ILayer*       getLayer(size_t index)       = 0;
    virtual const ILayer* getLayer(size_t index) const = 0;

    virtual ILayer*       getTempLayer()       = 0;
    virtual const ILayer* getTempLayer() const = 0;

    virtual void cleanTempLayer() = 0;

    virtual size_t getNumLayers() const = 0;

    virtual size_t getActiveLayerIndex() const = 0;
    virtual void   setActiveLayerIndex(size_t index) = 0;

    virtual bool insertLayer     (size_t index, std::unique_ptr<ILayer> layer) = 0;
    virtual bool removeLayer     (size_t index) = 0;
    virtual bool insertEmptyLayer(size_t index) = 0;

    virtual void setPos  (sfm::vec2i pos)   = 0;
    virtual void setSize (sfm::vec2i size)  = 0;
    virtual void setScale(sfm::vec2f scale) = 0;

    virtual sfm::vec2i getMousePosition() const = 0;
    virtual bool isPressed()         const = 0;
};

} // namespace

#endif // API_CANVAS_HPP
