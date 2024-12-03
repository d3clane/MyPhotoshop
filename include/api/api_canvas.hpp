#ifndef API_CANVAS_HPP
#define API_CANVAS_HPP

#include "api_photoshop.hpp"
#include "api_sfm.hpp"
#include "api_memento.hpp"

#include <cstdint>
#include <memory>

namespace psapi {

const wid_t kCanvasWindowId = 100;

using drawable_id_t = int64_t;

class ILayer : public IMementable<ILayerSnapshot>
{
public:
    virtual ~ILayer() = default;

    virtual sfm::Color getPixel(sfm::vec2i pos) const = 0;
    virtual void       setPixel(sfm::vec2i pos, sfm::Color pixel) = 0;

    /**
     * @brief This functions adds drawable object and returns id of added shape
     */
    virtual drawable_id_t addDrawable(std::unique_ptr<sfm::Drawable> object) = 0;

    /**
     * @brief Removes drawable object using id. Other id stay valid
     */
    virtual void removeDrawable(drawable_id_t id) = 0;

    /**
     * @brief Removes all drawable objects. All id become invalid
     */
    virtual void removeAllDrawables() = 0;

    virtual sfm::vec2u getSize() const = 0;
};

class ICanvas : public IWindow, public IMementable<ICanvasSnapshot>
{
public:
    virtual ~ICanvas() = default;

    /**
     * @brief Get layer pointer on specified index
     */
    virtual ILayer*       getLayer(size_t index)       = 0;
    virtual const ILayer* getLayer(size_t index) const = 0;

    /**
     * @brief Get temp layer pointer
     */
    virtual ILayer*       getTempLayer()       = 0;
    virtual const ILayer* getTempLayer() const = 0;

    /**
     * @brief Fill temp layer with canvas base color
     */
    virtual void cleanTempLayer() = 0;

    /**
     * @brief Get the number of layers
     */
    virtual size_t getNumLayers() const = 0;

    /**
     * @brief Get or set the Active Layer Index
     */
    virtual size_t getActiveLayerIndex() const = 0;
    virtual void   setActiveLayerIndex(size_t index) = 0;

    /**
     * @brief Inserts layer after specified index
     */
    virtual bool insertLayer(size_t index, std::unique_ptr<ILayer> layer) = 0;

    /**
     * @brief Removes layer on specified index
     */
    virtual bool removeLayer     (size_t index) = 0;

    /**
     * @brief Inserts empty layer filled with canvas base color, which you can get with getCanvasBaseColor
     *
     * @param index - index of layer after which new layer will be inserted
     */
    virtual bool insertEmptyLayer(size_t index) = 0;

    /**
     * @brief Zoom canvas
     */
    virtual void setZoom(sfm::vec2f zoom) = 0;

    /**
     * @brief Get the position of mouse relative to canvas
     */
    virtual sfm::vec2i getMousePosition() const = 0;

    /**
     * @brief Check if left/right/scroll mouse button is pressed on canvas
     */
    virtual bool isPressedRightMouseButton() const = 0;
    virtual bool isPressedLeftMouseButton()  const = 0;
    virtual bool isPressedScrollButton()     const = 0;

    /**
     * @brief Get the color of the empty canvas
     */
    virtual sfm::Color getCanvasBaseColor() const = 0;
};

} // namespace

#endif // API_CANVAS_HPP

