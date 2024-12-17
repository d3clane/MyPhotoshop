#ifndef INTERFACE_INFO
#define INTERFACE_INFO

#include "api/api_sfm.hpp"

namespace ps
{

psapi::sfm::Color getCommonBarColor();

const char* getCommonHoverTexturePath  ();
const char* getCommonPressTexturePath  ();
const char* getCommonReleaseTexturePath();

psapi::sfm::IntRect getLayersIntRect();
psapi::sfm::IntRect getToolOptionsRightIntRect();
psapi::sfm::IntRect getUpperToolOptionsIntRect();

} // namespace ps

#endif // INTERFACE_INFO