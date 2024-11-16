#ifndef API_IMPLEMENTATION_PS_CANVAS_H_
#define API_IMPLEMENTATION_PS_CANVAS_H_

#include "api/api_canvas.hpp"
#include "api/api_sfm.hpp"
#include "windows/windows.hpp"
#include "scrollbar/scrollbar.hpp"

#include <iostream>

extern "C"
{

bool loadPlugin();
void unloadPlugin();

}

namespace ps {

using namespace psapi;
using namespace psapi::sfm;

struct CutRect
{
    vec2i pos;
    vec2u size;
};

class Layer : public ILayer
{
public:
    explicit Layer(vec2u size);
    Color getPixel(vec2i pos) const override;
    void  setPixel(vec2i pos, Color pixel) override;

private:
    friend class Canvas;
    
    vec2u fullSize_;
    CutRect area_;

    std::vector<Color> pixels_;

protected:
    void changeFullSize(vec2u size);
    void changeArea(const CutRect& area);
};

class Canvas : public ICanvas, public IScrollableWindow
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

    void setPos  (sfm::vec2i pos)   override;
    void setSize (sfm::vec2i size)  override;
    void setScale(sfm::vec2f scale) override;

    sfm::vec2i getMousePosition() const override;
    bool isPressed() const override;

    void draw(IRenderWindow* renderWindow) override;
    bool update(const IRenderWindow* renderWindow, const Event& event) override;

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

private:
    vec2u size_;
    vec2i pos_;

    const IWindow* parent_;

    bool isActive_ = true;

    size_t activeLayer_ = 0;

    std::unique_ptr<Layer> tempLayer_;
    std::vector<std::unique_ptr<Layer>> layers_;

    vec2f scale_ = {1.0f, 1.0f};

    std::unique_ptr<IRectangleShape> boundariesShape_;
    
    vec2i lastMousePosRelatively_ = {-1, -1};
    bool isPressed_ = false;

    vec2u fullSize_;

    vec2f scroll_ = {0.0f, 0.0f};

    // private functions
private:
    void drawLayer(const Layer& layer, IRenderWindow* renderWindow);
};

} // namespace

#endif  // API_IMPLEMENTATION_PS_CANVAS_H_
