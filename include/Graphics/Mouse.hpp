#ifndef MOUSE_HPP
#define MOUSE_HPP

#include <SFML/Graphics.hpp>

namespace Graphics
{

struct WindowPoint;
class RenderWindow;

namespace Mouse
{

enum class Button
{
    Left, Right, Middle, XButton1, XButton2,
};

enum class Wheel
{
    VerticalWheel, HorizontalWheel,
};

bool isButtonPressed(const Button& button);
WindowPoint getPosition(const RenderWindow& window);

} // namespace Mouse

} // namespace Graphics

#endif // MOUSE_HPP