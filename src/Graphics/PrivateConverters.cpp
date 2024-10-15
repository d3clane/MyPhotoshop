#include "PrivateConverters.hpp"

namespace Graphics
{

Event::KeyCode convertSfKeyCodeToKeyCode(sf::Keyboard::Key sfKeyCode) 
{
    switch (sfKeyCode)
    {
        case sf::Keyboard::Key::W:          return Event::KeyCode::W;
        case sf::Keyboard::Key::S:          return Event::KeyCode::S;
        case sf::Keyboard::Key::A:          return Event::KeyCode::A;
        case sf::Keyboard::Key::D:          return Event::KeyCode::D;
        case sf::Keyboard::Key::Q:          return Event::KeyCode::Q;
        case sf::Keyboard::Key::E:          return Event::KeyCode::E;
        default:                            return Event::KeyCode::None;
    }

    return Event::KeyCode::None;
}

sf::Mouse::Button convertButtonToSfButton(const Mouse::Button& button)
{
    switch (button)
    {
        case Mouse::Button::Left:      return sf::Mouse::Left;
        case Mouse::Button::Right:     return sf::Mouse::Right;
        case Mouse::Button::Middle:    return sf::Mouse::Middle;
        case Mouse::Button::XButton1:  return sf::Mouse::XButton1;
        case Mouse::Button::XButton2:  return sf::Mouse::XButton2;
        default:                assert(false); // unreachable 
    }

    assert(false); // unreachable
    return sf::Mouse::Left;
}

Mouse::Button convertSfButtonToButton(const sf::Mouse::Button& sfButton)
{
    switch (sfButton)
    {
        case sf::Mouse::Left:        return Mouse::Button::Left;
        case sf::Mouse::Right:       return Mouse::Button::Right;
        case sf::Mouse::Middle:      return Mouse::Button::Middle;
        case sf::Mouse::XButton1:    return Mouse::Button::XButton1;
        case sf::Mouse::XButton2:    return Mouse::Button::XButton2;

        case sf::Mouse::ButtonCount: 
        default:                     assert(false); // unreachable 
    }

    assert(false); // unreachable
    return Mouse::Button::Left;
}

sf::Event::MouseButtonEvent convertButtonEventToSfButtonEvent(const Event::MouseButtonEvent& button)
{
    sf::Event::MouseButtonEvent sfButtonEvent;

    sfButtonEvent.x      = button.x;
    sfButtonEvent.y      = button.y;
    sfButtonEvent.button = convertButtonToSfButton(button.button);

    return sfButtonEvent;
}

Event::MouseButtonEvent convertSfButtonEventToButtonEvent(const sf::Event::MouseButtonEvent& sfButton)
{
    Event::MouseButtonEvent buttonEvent;

    buttonEvent.x      = sfButton.x;
    buttonEvent.y      = sfButton.y;
    buttonEvent.button = convertSfButtonToButton(sfButton.button);

    return buttonEvent;
}

} // namespace Graphics