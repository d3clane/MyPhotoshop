#include "Graphics/Font.hpp"

namespace Graphics
{

bool Font::loadFromFile(const std::string& fileName)
{
    return font_.loadFromFile(fileName);
}

} // namespace Graphics