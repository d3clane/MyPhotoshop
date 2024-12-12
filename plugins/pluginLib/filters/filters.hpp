#ifndef PLUGINS_PLUGIN_LIB_FILTERS_FILTERS_HPP
#define PLUGINS_PLUGIN_LIB_FILTERS_FILTERS_HPP

#include "api/api_sfm.hpp"
#include "api/api_system.hpp"

#include <vector>

namespace ps
{

using namespace psapi;
using namespace psapi::sfm;

std::vector<std::vector<Color>> getNegative (const std::vector<std::vector<Color>>& pixels);
std::vector<std::vector<Color>> getBasRelief(const std::vector<std::vector<Color>>& pixels, 
                                             const std::vector<std::vector<Color>>& negative);

std::vector<std::vector<Color>> getBoxBlured(const std::vector<std::vector<Color>>& pixels);

} // namespace ps

#endif // PLUGINS_PLUGIN_LIB_FILTERS_FILTERS_HPP