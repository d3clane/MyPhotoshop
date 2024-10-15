#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include <SFML/Graphics.hpp>
#include "Graphics/Mouse.hpp"
#include "Graphics/Renderable.hpp"

namespace Graphics
{

class Event;

struct WindowPoint
{
    int x, y;

    WindowPoint(int x, int y) : x(x), y(y) {}
};

inline WindowPoint operator +(const WindowPoint& left, const WindowPoint& right)
{
    return WindowPoint(left.x + right.x, left.y + right.y);
}

inline WindowPoint operator -(const WindowPoint& point) 
{   
    return WindowPoint(-point.x, -point.y);
}

inline WindowPoint operator -(const WindowPoint& left, const WindowPoint& right)
{
    return WindowPoint(left.x - right.x, left.y - right.y);
}

using WindowVector = WindowPoint;

class RenderWindow;

struct WindowLine : public Renderable
{
    WindowPoint start, end;

    WindowLine(const WindowPoint& start, const WindowPoint& end) : start(start), end(end) {}

    void draw(RenderWindow& window) override;
};

class PixelsArray;
class Sprite;
class Circle;
class Text;

class RenderWindow
{
private:
    sf::RenderWindow renderWindow_;

public:
    const unsigned int width_, height_;

    RenderWindow(int width, int height, const char* title);

    bool isOpen();
    void close();
    void clear();

    void drawLine  (const WindowLine& line);
    void drawPixels(const PixelsArray& pixels);
    void drawSprite(const Sprite& sprite);
    void drawCircle(const Circle& circle);
    void drawText  (const Text& text);

    void display();

    unsigned int getWidth () const;
    unsigned int getHeight() const;

    bool pollEvent(Event& event);

    friend WindowPoint Mouse::getPosition(const RenderWindow& window);
};

} // Graphics

#endif // GRAPHICS_HPP