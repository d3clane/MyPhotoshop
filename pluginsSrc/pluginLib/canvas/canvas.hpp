#ifndef PLUGIN_LIB_CANVAS_CANVAS_HPP
#define PLUGIN_LIB_CANVAS_CANVAS_HPP

#include "api/api_canvas.hpp"
#include "api/api_sfm.hpp"

namespace ps
{

void copyLayerToLayer(psapi::ILayer* dst, const psapi::ILayer* src, const psapi::vec2u& size);
void copyImageToLayer(psapi::ILayer* dst, const psapi::sfm::IImage* src, 
                      const psapi::vec2i& layerPos, const psapi::vec2u& size);

} // namespace ps

#endif // PLUGIN_LIB_CANVAS_CANVAS_HPP