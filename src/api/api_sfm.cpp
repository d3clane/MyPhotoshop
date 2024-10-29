#include "sfm/sfm_impl.hpp"
#include "api/api_sfm.hpp"

namespace psapi
{

namespace sfm
{
    
std::unique_ptr<ITexture> ITexture::create()
{
    return std::make_unique<Texture>();
}

std::unique_ptr<ISprite> ISprite::create()
{
    return std::make_unique<Sprite>();
}

std::unique_ptr<IRenderWindow> IRenderWindow::create(unsigned int width, unsigned int height, 
                                                     const std::string& name)
{
    return std::make_unique<RenderWindow>(width, height, name);
}

bool Mouse::isButtonPressed(Button button)
{
    assert(false);
    return sf::Mouse::isButtonPressed(static_cast<sf::Mouse::Button>(button));
}

vec2i Mouse::getPosition()
{
    auto mousePos = sf::Mouse::getPosition();
    return {mousePos.x, mousePos.y};
}

vec2i Mouse::getPosition(const IRenderWindow *relative_to)
{
    const RenderWindow* renderWindow = static_cast<const RenderWindow*>(relative_to);
    auto mousePos = sf::Mouse::getPosition(renderWindow->window_);

    return {mousePos.x, mousePos.y};
}

void Mouse::setPosition(const vec2i &position)
{
    sf::Mouse::setPosition({position.x, position.y});
}

void Mouse::setPosition(const vec2i &position, const IRenderWindow *relative_to)
{
    const RenderWindow* renderWindow = static_cast<const RenderWindow*>(relative_to);
    sf::Mouse::setPosition({position.x, position.y}, renderWindow->window_);
}

} // namespace sfm

} // namespace psapi