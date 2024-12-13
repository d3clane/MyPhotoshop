#ifndef PLUGIN_LIB_CANVAS_CANVAS_HPP
#define PLUGIN_LIB_CANVAS_CANVAS_HPP

#include "api/api_canvas.hpp"
#include "api/api_sfm.hpp"

#include <vector>

namespace ps
{

using namespace psapi;
using namespace psapi::sfm;

class CanvasSaverAction : public IUndoableAction
{
public:
    CanvasSaverAction() = default;
    CanvasSaverAction(std::unique_ptr<ICanvasSnapshot> pastSnapshot,
                      std::unique_ptr<ICanvasSnapshot> futureSnapshot);

    bool isUndoable(const Key& key) override;
    bool execute(const Key& key) override;
    bool undo(const Key& key) override;
    bool redo(const Key& key) override;

    void setPastSnapshot  (std::unique_ptr<ICanvasSnapshot> snapshot);
    void setFutureSnapshot(std::unique_ptr<ICanvasSnapshot> snapshot);

private:
    std::unique_ptr<ICanvasSnapshot> pastSnapshot_;
    std::unique_ptr<ICanvasSnapshot> futureSnapshot_;
};

void copyLayerToLayer(ILayer* dst, const ILayer* src, const vec2u& size);
void copyImageToLayer(ILayer* dst, const IImage* src, const vec2i& layerPos);
std::unique_ptr<IImage> copyLayerToImage(const ILayer* src, const vec2u& size);

std::vector<std::vector<Color>> getLayerScreenIn2D(const ILayer* layer, const vec2u& size);
std::vector<Color> getLayerScreenIn1D(const ILayer* layer, const vec2u& size);

void copyPixelsToLayer(ILayer* layer, const std::vector<std::vector<Color>>& pixels);

} // namespace ps

#endif // PLUGIN_LIB_CANVAS_CANVAS_HPP