#ifndef API_IMPLEMENTATION_PS_CANVAS_H_
#define API_IMPLEMENTATION_PS_CANVAS_H_

#include "api/api_canvas.hpp"
#include "api/api_sfm.hpp"
#include "pluginLib/windows/windows.hpp"
#include "pluginLib/scrollbar/scrollbar.hpp"

#include <iostream>

extern "C"
{

bool onLoadPlugin();
void onUnloadPlugin();

}

namespace ps {

using namespace psapi;
using namespace psapi::sfm;

struct CutRect
{
    vec2i pos;
    vec2u size;
};

class Layer;

class LayerSnapshot : public ILayerSnapshot
{
public:
    LayerSnapshot(const std::vector<std::shared_ptr<Drawable>>& drawables, 
                  const std::vector<Color>& pixels);

    std::vector<std::shared_ptr<Drawable>> getDrawables() const;
    std::vector<Color> getPixels() const;

private:
    std::vector<std::shared_ptr<Drawable>> drawables_;
    std::vector<Color> pixels_;
};

class Layer : public ILayer
{
public:
    Layer(vec2u size, vec2u fullSize);
    Color getPixel(vec2i pos) const override;
    void  setPixel(vec2i pos, Color pixel) override;

    drawable_id_t addDrawable(std::unique_ptr<Drawable> object) override;
    void removeDrawable(drawable_id_t id) override;
    void removeAllDrawables() override;

    vec2u getSize() const override;

    std::unique_ptr<ILayerSnapshot> save() override;
    void restore(ILayerSnapshot* snapshot) override;

private:
    friend class Canvas;
    
    vec2u size_;
    vec2u fullSize_;
    CutRect area_;

    std::vector<Color> pixels_;
    std::vector<std::shared_ptr<Drawable>> drawables_; // shared ptr because of snapshots(
    // TODO: value semantics problem
protected:
    void changeFullSize(vec2u size);
    void changeArea(const CutRect& area);
};

class Canvas;

class CanvasSnapshot : public ICanvasSnapshot
{
public:
    CanvasSnapshot(std::unique_ptr<LayerSnapshot> tempLayer, 
                   std::vector<std::unique_ptr<LayerSnapshot>>&& layers);
    
    LayerSnapshot* getTempLayerSnapshot() const;
    std::vector<LayerSnapshot*> getLayersSnapshots() const;

private:
    std::unique_ptr<LayerSnapshot> tempLayer_;
    std::vector<std::unique_ptr<LayerSnapshot>> layers_;
};

class Canvas : public ICanvas, public IScrollable
{
public:
    Canvas(vec2i pos, vec2u size);
    ~Canvas() = default;

    ILayer*       getLayer(size_t index)       override;
    const ILayer* getLayer(size_t index) const override;

    ILayer*       getTempLayer()       override;
    const ILayer* getTempLayer() const override;

    void cleanTempLayer() override;

    size_t getNumLayers() const override;

    size_t getActiveLayerIndex() const override;
    void   setActiveLayerIndex(size_t index) override;

    bool insertLayer     (size_t index, std::unique_ptr<ILayer> layer) override;
    bool removeLayer     (size_t index) override;
    bool insertEmptyLayer(size_t index) override;

    vec2i getMousePosition() const override;

    void setPos  (const vec2i& pos)  override;
    void setSize (const vec2u& size) override;

    void setZoom(vec2f zoom) override;

    bool isPressedRightMouseButton() const override;
    bool isPressedLeftMouseButton()  const override;
    bool isPressedScrollButton()     const override;

    Color getCanvasBaseColor() const override;

    void draw(IRenderWindow* renderWindow) override;
    std::unique_ptr<IAction> createAction(const IRenderWindow* renderWindow, 
                                          const Event& event) override;
    bool update(const IRenderWindow* renderWindow, const Event& event);

    wid_t getId() const override;

    IWindow* getWindowById(wid_t id) override;

    const IWindow* getWindowById(wid_t id) const override;

    vec2i getPos() const override;
    vec2u getSize() const override;

    void setParent(const IWindow* parent) override;

    void forceActivate() override;
    void forceDeactivate() override;

    bool isActive() const override;
    bool isWindowContainer() const override;

    void scroll(vec2f delta) override;
    void setScroll(vec2f scroll) override;

    vec2f getScroll() override;

    vec2u getVisibleSize() override;
    vec2u getFullSize() override;

    std::unique_ptr<ICanvasSnapshot> save() override;
    void restore(ICanvasSnapshot* snapshot) override;

private:
    enum class PressType
    {
        LMB = 1,
        RMB = 2,
        MMB = 4, // MIDDLE MOUSE BUTTON
    };

private:
    vec2u size_;
    vec2i pos_;
    vec2u fullSize_;

    const IWindow* parent_;

    bool isActive_ = true;

    size_t activeLayer_ = 0;

    std::unique_ptr<Layer> tempLayer_;
    std::vector<std::unique_ptr<Layer>> layers_;

    vec2i lastMousePosRelatively_ = {-1, -1};
    std::unique_ptr<IRectangleShape> boundariesShape_;

    vec2f zoom_ = {1.0f, 1.0f};
    vec2f scroll_ = {0.0f, 0.0f};

    uint8_t pressType_ = 0;

    // private functions
private:
    void drawLayer(const Layer& layer, IRenderWindow* renderWindow);
    void drawPixels(const Layer& layer, IRenderWindow* renderWindow);
    void drawDrawables(const Layer& layer, IRenderWindow* renderWindow);
    
    uint8_t updatePressType(uint8_t pressType, const Event& event);
};

} // namespace

#endif  // API_IMPLEMENTATION_PS_CANVAS_H_
