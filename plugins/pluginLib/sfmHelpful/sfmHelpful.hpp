#ifndef PLUGINS_PLUGIN_LIB_SFML_HELPFUL_HPP
#define PLUGINS_PLUGIN_LIB_SFML_HELPFUL_HPP

#include "api/api_sfm.hpp"

namespace ps
{

struct SpriteInfo
{
    std::unique_ptr<psapi::sfm::ISprite> sprite;
    std::unique_ptr<psapi::sfm::ITexture> texture;
};

SpriteInfo createSprite(const psapi::sfm::vec2u& size, const char* filename);
std::unique_ptr<psapi::sfm::IRectangleShape> createShape(psapi::sfm::Color color, const psapi::sfm::vec2u& size);

}
#endif // PLUGINS_PLUGIN_LIB_SFML_HELPFUL_HPP