#ifndef PLUGIN_LIB_CANVAS_CANVAS_HPP
#define PLUGIN_LIB_CANVAS_CANVAS_HPP

#include "api/api_canvas.hpp"
#include "api/api_sfm.hpp"

#include <vector>

namespace ps
{

using namespace psapi;
using namespace psapi::sfm;

void copyLayerToLayer(ILayer* dst, const ILayer* src, const vec2u& size);
void copyImageToLayer(ILayer* dst, const IImage* src, 
                      const vec2i& layerPos, const vec2u& size);

std::vector<std::vector<Color>> getLayerScreenIn2D(const ILayer* layer, const vec2u& size);
std::vector<Color> getLayerScreenIn1D(const ILayer* layer, const vec2u& size);

void copyPixelsToLayer(ILayer* layer, const std::vector<std::vector<Color>>& pixels);

} // namespace ps

#endif // PLUGIN_LIB_CANVAS_CANVAS_HPP