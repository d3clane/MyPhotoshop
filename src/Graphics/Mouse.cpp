#include <SFML/Graphics.hpp>

#include "Graphics/GraphicsWindow.hpp"
#include "Graphics/Mouse.hpp"

#include "PrivateConverters.hpp"

namespace Graphics
{

namespace Mouse
{

bool isButtonPressed(const Button& button)
{
    return sf::Mouse::isButtonPressed(convertButtonToSfButton(button));
}

WindowPoint getPosition(const RenderWindow& window)
{
    sf::Vector2i sfMousePosition = sf::Mouse::getPosition(window.renderWindow_);

    return WindowPoint(sfMousePosition.x, sfMousePosition.y);
}

} // namespace Mouse

} // namespace Graphics