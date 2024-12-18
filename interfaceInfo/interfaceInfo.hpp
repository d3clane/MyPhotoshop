#ifndef INTERFACE_INFO
#define INTERFACE_INFO

#include "api/api_sfm.hpp"

namespace ps
{

unsigned getCommonTextCharacterSize();

psapi::sfm::Color getCommonBarColor();

const char* getCommonHoverTexturePath  ();
const char* getCommonPressTexturePath  ();
const char* getCommonReleaseTexturePath();

psapi::sfm::IntRect getLayersIntRect();
psapi::sfm::IntRect getToolOptionsRightIntRect();
psapi::sfm::IntRect getToolOptionsUpIntRect();

} // namespace ps

#endif // INTERFACE_INFO