#include "sfm/sfm_impl.hpp"

#include <SFML/Graphics.hpp>
#include <memory>

#include <iostream>
#include <assert.h>

#include <algorithm>

namespace psapi
{

namespace sfm
{

// RenderWindow implementation
RenderWindow::RenderWindow(unsigned int width, unsigned int height, const std::string& title)
    : window_(sf::VideoMode(width, height), title) 
{
}

bool RenderWindow::isOpen() const 
{
    return window_.isOpen();
}

void RenderWindow::clear() 
{
    window_.clear();
}

void RenderWindow::display() 
{
    window_.display();
}

void RenderWindow::close() 
{
    window_.close();
}

vec2u RenderWindow::getSize() const 
{
    return { window_.getSize().x, window_.getSize().y };
}

bool RenderWindow::pollEvent(Event& event) 
{
    sf::Event sfEvent;

    if (!window_.pollEvent(sfEvent))
    {
        event.type = Event::None;
        return false;
    }

    switch (sfEvent.type)
    {
        case sf::Event::Closed:
            event.type = Event::Closed;
            break;

        case sf::Event::Resized:
            event.type = Event::Resized;
            event.size.width  = sfEvent.size.width;
            event.size.height = sfEvent.size.height;
            break;

        case sf::Event::LostFocus:
            event.type = Event::LostFocus;
            break;

        case sf::Event::GainedFocus:
            event.type = Event::GainedFocus;
            break;

        case sf::Event::TextEntered:
            event.type = Event::TextEntered;
            event.text.unicode = sfEvent.text.unicode;
            break;

        case sf::Event::KeyPressed:
            event.type = Event::KeyPressed;
            event.key.code = static_cast<Keyboard::Key>(sfEvent.key.code);
            event.key.alt     = sfEvent.key.alt;
            event.key.control = sfEvent.key.control;
            event.key.shift   = sfEvent.key.shift;
            event.key.system  = sfEvent.key.system;
            break;

        case sf::Event::KeyReleased:
            event.type = Event::KeyReleased;
            event.key.code = static_cast<Keyboard::Key>(sfEvent.key.code);
            event.key.alt     = sfEvent.key.alt;
            event.key.control = sfEvent.key.control;
            event.key.shift   = sfEvent.key.shift;
            event.key.system  = sfEvent.key.system;
            break;

        case sf::Event::MouseWheelScrolled:
            event.type = Event::MouseWheelScrolled;

            event.mouseWheel.wheel = sfEvent.mouseWheelScroll.wheel == sf::Mouse::HorizontalWheel ? 
                                     Mouse::Wheel::Horizontal : Mouse::Wheel::Vertical;

            event.mouseWheel.delta = sfEvent.mouseWheelScroll.delta;
            event.mouseWheel.x     = sfEvent.mouseWheelScroll.x;
            event.mouseWheel.y     = sfEvent.mouseWheelScroll.y;
            break;

        case sf::Event::MouseButtonPressed:
            event.type = Event::MouseButtonPressed;
            event.mouseButton.button = static_cast<Mouse::Button>(sfEvent.mouseButton.button);
            event.mouseButton.x = sfEvent.mouseButton.x;
            event.mouseButton.y = sfEvent.mouseButton.y;
            break;

        case sf::Event::MouseButtonReleased:
            event.type = Event::MouseButtonReleased;
            event.mouseButton.button = static_cast<Mouse::Button>(sfEvent.mouseButton.button);
            event.mouseButton.x = sfEvent.mouseButton.x;
            event.mouseButton.y = sfEvent.mouseButton.y;
            break;

        case sf::Event::MouseMoved:
            event.type = Event::MouseMoved;
            event.mouseMove.x = sfEvent.mouseMove.x;
            event.mouseMove.y = sfEvent.mouseMove.y;
            break;

        case sf::Event::MouseEntered:
            event.type = Event::MouseEntered;
            break;

        case sf::Event::MouseLeft:
            event.type = Event::MouseLeft;
            break;

        case sf::Event::MouseWheelMoved: // deprecated
        case sf::Event::JoystickButtonPressed:
        case sf::Event::JoystickButtonReleased:
        case sf::Event::JoystickMoved:
        case sf::Event::JoystickConnected:
        case sf::Event::JoystickDisconnected:
        case sf::Event::TouchBegan:
        case sf::Event::TouchMoved:
        case sf::Event::TouchEnded:
        case sf::Event::SensorChanged:
        case sf::Event::Count:
        default:
            event.type = Event::Unknown;
            break;
    }

    return true;
}

void RenderWindow::draw(Drawable* target) 
{
    if (target)
        target->draw(this);
}

void RenderWindow::setFps(float fps)
{
    window_.setFramerateLimit(static_cast<unsigned>(fps));
}

float RenderWindow::getFps() const
{
    std::cerr << "CAN'T GET FPS RIGHT NOW, NOT IMPLEMENTED\n";
    assert(false);

    return 0.0f;
}

bool Texture::create(unsigned int width, unsigned int height) 
{
    return texture_.create(std::max(1u, width), std::max(1u, height));
}

bool Texture::loadFromFile(const std::string& filename, const IntRect& area) 
{
    sf::IntRect sfArea(area.pos.x, area.pos.y, (int)area.size.x, (int)area.size.y);

    return texture_.loadFromFile(filename, sfArea);
}

bool Texture::loadFromMemory(const void* data, std::size_t size, const IntRect& area) 
{
    sf::IntRect sfArea(area.pos.x, area.pos.y, (int)area.size.x, (int)area.size.y);

    return texture_.loadFromMemory(data, size, sfArea);
}

vec2u Texture::getSize() const 
{
    return { texture_.getSize().x, texture_.getSize().y };
}

void Texture::update(const Color* pixels)
{
    assert(pixels);

    texture_.update(reinterpret_cast<const sf::Uint8*>(pixels));
}

void Texture::update(const Color* pixels, 
                     unsigned int width, unsigned int height, 
                     unsigned int x, unsigned int y) 
{
    assert(pixels || (width == 0 || height == 0));

    texture_.update(reinterpret_cast<const sf::Uint8*>(pixels), width, height, x, y);
}

void Texture::update(const IImage* image) 
{
    const Image* sfmlImage = static_cast<const Image*>(image);
    texture_.update(sfmlImage->image_);
}

std::unique_ptr<IImage> Texture::copyToImage() const
{
    Image* img = new Image();
    img->image_ = texture_.copyToImage();

    return std::unique_ptr<IImage>(img);
}

void Sprite::setTexture(const ITexture* texture, bool reset_rect) 
{
    const Texture* sfmTexture = static_cast<const Texture*>(texture);
    sprite_.setTexture(sfmTexture->texture_, reset_rect);
}

void Sprite::setTextureRect(const IntRect& rectangle) 
{
    sprite_.setTextureRect(sf::IntRect(static_cast<int>(rectangle.pos.x),
                                       static_cast<int>(rectangle.pos.y),
                                       static_cast<int>(rectangle.size.x),
                                       static_cast<int>(rectangle.size.y)));
}

void Sprite::setPosition(float x, float y) 
{
    sprite_.setPosition(x, y);
}

void Sprite::setPosition(const vec2f& pos) 
{
    sprite_.setPosition(pos.x, pos.y);
}

void Sprite::setScale(float factorX, float factorY) 
{
    sprite_.setScale(factorX, factorY);
}

vec2u Sprite::getSize() const 
{
    sf::Vector2u size = sprite_.getTexture()->getSize();

    return vec2u{ static_cast<unsigned>(static_cast<float>(size.x) * sprite_.getScale().x), 
                  static_cast<unsigned>(static_cast<float>(size.y) * sprite_.getScale().y) };
}

void Sprite::setColor(const Color& color) 
{
    sprite_.setColor(sf::Color(color.r, color.g, color.b, color.a));
}

Color Sprite::getColor() const
{
    sf::Color color = sprite_.getColor();
    return { color.r, color.g, color.b, color.a };
}

void Sprite::setRotation(float angle) 
{
    sprite_.setRotation(angle);
}

const vec2f Sprite::getPosition() const
{
    sf::Vector2f pos = sprite_.getPosition();
    return { pos.x, pos.y };
}

IntRect Sprite::getGlobalBounds() const 
{
    sf::FloatRect bounds = sprite_.getGlobalBounds();
    return { vec2i{static_cast<int>     (bounds.left ), static_cast<int>     (bounds.top   )},
             vec2u{static_cast<unsigned>(bounds.width), static_cast<unsigned>(bounds.height)} };
}

void Sprite::draw(IRenderWindow* window) const
{
    RenderWindow* sfmWindow = static_cast<RenderWindow*>(window);
    sfmWindow->window_.draw(sprite_);
}

// Font implementation 

bool Font::loadFromFile(const std::string& filename) 
{
    return font_.loadFromFile(filename);
}

// Text implementation

void Text::draw(IRenderWindow* window) const 
{
    RenderWindow* sfmlWindow = static_cast<RenderWindow*>(window);
    sfmlWindow->window_.draw(text_);
}

void Text::setString(const std::string& string) 
{
    text_.setString(string);
}

void Text::setFont(const IFont* font) 
{
    const Font* sfmlFont = static_cast<const Font*>(font);
    text_.setFont(sfmlFont->font_);
}

void Text::setCharacterSize(unsigned int size) 
{
    text_.setCharacterSize(size);
}

void Text::setStyle(uint32_t style) 
{
    text_.setStyle(style);
}

void Text::setFillColor(const Color* color) 
{
    text_.setFillColor(sf::Color(color->r, color->g, color->b, color->a));
}

void Text::setOutlineColor(const Color* color) 
{
    text_.setOutlineColor(sf::Color(color->r, color->g, color->b, color->a));
}

void Text::setOutlineThickness(float thickness) 
{
    text_.setOutlineThickness(thickness);
}

IntRect Text::getGlobalBounds() const
{
    sf::FloatRect bounds = text_.getGlobalBounds();
    return { vec2i{static_cast<int>     (bounds.left ), static_cast<int>     (bounds.top   )},
             vec2u{static_cast<unsigned>(bounds.width), static_cast<unsigned>(bounds.height)} };
}

void Text::setPos(const vec2f &pos)
{
    text_.setPosition(pos.x, pos.y);
}

void Text::setSize(const vec2f &size)
{
    text_.setScale(sf::Vector2f{1.f, 1.f});

    sf::FloatRect bounds = text_.getGlobalBounds();
    text_.setScale(sf::Vector2f{static_cast<float>(size.x) / bounds.width,
                                static_cast<float>(size.y) / bounds.height});
}

// Image implementation

void Image::create(unsigned int width, unsigned int height, const Color& color) 
{
    sf::Color sfmlColor{color.r, color.g, color.b, color.a};
    image_.create(width, height, sfmlColor);
}

void Image::create(vec2u size, const Color& color) 
{
    create(size.x, size.y, color);
}

void Image::create(unsigned int width, unsigned int height, const Color* pixels) 
{
    image_.create(width, height, reinterpret_cast<const sf::Uint8*>(pixels));
}

void Image::create(vec2u size, const Color* pixels) 
{
    create(size.x, size.y, pixels);
}

bool Image::loadFromFile(const std::string& filename) 
{
    return image_.loadFromFile(filename);
}

vec2u Image::getSize() const 
{
    sf::Vector2u size = image_.getSize();
    return vec2u{size.x, size.y};
}

void Image::setPixel(unsigned int x, unsigned int y, const Color& color) 
{
    sf::Color sfmlColor{color.r, color.g, color.b, color.a};
    image_.setPixel(x, y, sfmlColor);
}

void Image::setPixel(vec2u pos, const Color& color) 
{
    setPixel(pos.x, pos.y, color);
}

Color Image::getPixel(unsigned int x, unsigned int y) const 
{
    sf::Color sfmlColor = image_.getPixel(x, y);
    return Color{sfmlColor.r, sfmlColor.g, sfmlColor.b, sfmlColor.a};
}

Color Image::getPixel(vec2u pos) const 
{
    return getPixel(pos.x, pos.y);
}

std::unique_ptr<IImage> IImage::create() 
{
    return std::make_unique<Image>();
}

vec2i Image::getPos() const
{
    return pos_;
}

void Image::setPos(const vec2i &pos)
{
    pos_ = pos;
}

// Rectangle implementation

RectangleShape::RectangleShape(unsigned int width, unsigned int height)
    : shape_(sf::Vector2f(static_cast<float>(width),
                          static_cast<float>(height))) 
{
}

RectangleShape::RectangleShape(const vec2u& size)
    : shape_(sf::Vector2f(static_cast<float>(size.x), 
                          static_cast<float>(size.y))) 
{
}

void RectangleShape::draw(IRenderWindow* window) const 
{
    auto sfmlWindow = static_cast<RenderWindow*>(window);
    sfmlWindow->window_.draw(shape_);
}

void RectangleShape::setTexture(const ITexture* texture) 
{
    const auto sfmlTexture = static_cast<const Texture*>(texture);
    shape_.setTexture(&sfmlTexture->texture_);
    imageNeedsUpdate_ = true;
}

void RectangleShape::setFillColor(const Color& color) 
{
    shape_.setFillColor(sf::Color{color.r, color.g, color.b, color.a});
    imageNeedsUpdate_ = true;
}

void RectangleShape::setPosition(const vec2i& pos) 
{
    shape_.setPosition(sf::Vector2f{static_cast<float>(pos.x),
                                    static_cast<float>(pos.y)});
    imageNeedsUpdate_ = true;
}

void RectangleShape::setPosition(const vec2f& pos) 
{
    shape_.setPosition(sf::Vector2f{pos.x, pos.y});
    imageNeedsUpdate_ = true;
}

void RectangleShape::setPosition(const vec2d& pos) 
{
    shape_.setPosition(sf::Vector2f{static_cast<float>(pos.x), static_cast<float>(pos.y)});
    imageNeedsUpdate_ = true;
}

void RectangleShape::setScale(const vec2f& scale) 
{
    shape_.setScale(sf::Vector2f{scale.x, scale.y});
    imageNeedsUpdate_ = true;
}

void RectangleShape::setSize(const vec2u& size) 
{
    shape_.setSize(sf::Vector2f{static_cast<float>(size.x),
                                static_cast<float>(size.y)});
    imageNeedsUpdate_ = true;
}

void RectangleShape::setRotation(float angle) 
{
    shape_.setRotation(angle);
    imageNeedsUpdate_ = true;
}

void RectangleShape::setOutlineColor(const Color& color) 
{
    shape_.setOutlineColor(sf::Color{color.r, color.g, color.b, color.a});
    imageNeedsUpdate_ = true;
}

void RectangleShape::setOutlineThickness(float thickness) 
{
    shape_.setOutlineThickness(thickness);
    imageNeedsUpdate_ = true;
}

float RectangleShape::getRotation() const 
{
    return shape_.getRotation();
}

vec2f RectangleShape::getScale() const 
{
    auto scale = shape_.getScale();
    return {scale.x, scale.y};
}

vec2f RectangleShape::getPosition() const 
{
    auto pos = shape_.getPosition();
    return {pos.x, pos.y};
}

const Color& RectangleShape::getFillColor() const 
{
    return reinterpret_cast<const Color&>(shape_.getFillColor());
}

vec2u RectangleShape::getSize() const 
{
    auto size = shape_.getSize();
    return {static_cast<unsigned int>(size.x), static_cast<unsigned int>(size.y)};
}

float RectangleShape::getOutlineThickness() const 
{
    return shape_.getOutlineThickness();
}

const Color& RectangleShape::getOutlineColor() const 
{
    return reinterpret_cast<const Color&>(shape_.getOutlineColor());
}

const IImage* RectangleShape::getImage() const 
{
    if (imageNeedsUpdate_)
        updateImage();

    return cachedImage_.get();
}

void RectangleShape::move(const vec2f& offset) 
{
    shape_.move(sf::Vector2f{offset.x, offset.y});
    imageNeedsUpdate_ = true;
}

void RectangleShape::updateImage() const 
{
    sf::RenderTexture renderTexture;
    
    sf::FloatRect shapeBounds = shape_.getGlobalBounds();
    
    vec2u totalSize = { static_cast<unsigned>(shapeBounds.width  + shapeBounds.left + 1), 
                        static_cast<unsigned>(shapeBounds.height + shapeBounds.top  + 1) };

    bool createResult = renderTexture.create(totalSize.x, totalSize.y);
    assert(createResult);

    renderTexture.clear(sf::Color::Transparent);
    renderTexture.draw(shape_);
    renderTexture.display();

    sf::Image image = renderTexture.getTexture().copyToImage();

    cachedImage_ = std::make_unique<Image>();
    cachedImage_->create(image.getSize().x, image.getSize().y,
                         reinterpret_cast<const Color*>(image.getPixelsPtr()));

    cachedImage_->setPos(vec2i{0, 0});

    imageNeedsUpdate_ = false;
}

std::unique_ptr<IRectangleShape> IRectangleShape::create(unsigned int width,
                                                         unsigned int height) 
{
    return std::make_unique<RectangleShape>(width, height); 
}

std::unique_ptr<IRectangleShape> IRectangleShape::create(const vec2u& size) 
{
    return std::make_unique<RectangleShape>(size);
}

// Ellipse implementation

EllipseShape::EllipseShape(unsigned int width, unsigned int height)
    : shape_(static_cast<float>(width) / 2.f) 
{
    setSize({width, height});
}

EllipseShape::EllipseShape(const vec2u& size)
    : EllipseShape(size.x, size.y) 
{
}

EllipseShape::EllipseShape(unsigned int radius)
    : shape_(static_cast<float>(radius)),
      imageNeedsUpdate_(true) 
{
}

void EllipseShape::draw(IRenderWindow *window) const 
{
    auto sfmlWindow = static_cast<RenderWindow*>(window);
    sfmlWindow->window_.draw(shape_);
}

void EllipseShape::setTexture(const ITexture *texture) 
{
    const auto sfmlTexture = static_cast<const Texture*>(texture);
    shape_.setTexture(&sfmlTexture->texture_);
    imageNeedsUpdate_ = true;
}

void EllipseShape::setFillColor(const Color& color)
{
    shape_.setFillColor(sf::Color(color.r, color.g, color.b, color.a));
    imageNeedsUpdate_ = true;
}

void EllipseShape::setPosition(const vec2i& pos)
{
    shape_.setPosition(sf::Vector2f(static_cast<float>(pos.x),
                                    static_cast<float>(pos.y)));
    imageNeedsUpdate_ = true;
}

void EllipseShape::setPosition(const vec2f& pos)
{
    shape_.setPosition(sf::Vector2f(pos.x, pos.y));
    imageNeedsUpdate_ = true;
}

void EllipseShape::setPosition(const vec2d& pos)
{
    shape_.setPosition(sf::Vector2f(static_cast<float>(pos.x), static_cast<float>(pos.y)));
    imageNeedsUpdate_ = true;
}

void EllipseShape::setScale(const vec2f& scale)
{
    shape_.setScale(sf::Vector2f(scale.x, scale.y));
    imageNeedsUpdate_ = true;
}

void EllipseShape::setSize(const vec2u& size)
{
    shape_.setRadius(static_cast<float>(size.x) / 2.f);
    shape_.setScale(1.f, static_cast<float>(size.y) / static_cast<float>(size.x));
    imageNeedsUpdate_ = true;
}

void EllipseShape::setRotation(float angle)
{
    shape_.setRotation(angle);
    imageNeedsUpdate_ = true;
}

void EllipseShape::setOutlineColor(const Color& color)
{
    shape_.setOutlineColor(sf::Color(color.r, color.g, color.b, color.a));
    imageNeedsUpdate_ = true;
}

void EllipseShape::setOutlineThickness(float thickness)
{
    shape_.setOutlineThickness(thickness);
    imageNeedsUpdate_ = true;
}

float EllipseShape::getRotation() const
{
    return shape_.getRotation();
}

vec2f EllipseShape::getScale() const
{
    auto scale = shape_.getScale();
    return {scale.x, scale.y};
}

vec2f EllipseShape::getPosition() const
{
    auto pos = shape_.getPosition();
    return {pos.x, pos.y};
}

const Color& EllipseShape::getFillColor() const
{
    return reinterpret_cast<const Color&>(shape_.getFillColor());
}

vec2u EllipseShape::getSize() const
{
    auto radius = shape_.getRadius();
    return {static_cast<unsigned int>(radius * 2.0f * shape_.getScale().x),
            static_cast<unsigned int>(radius * 2.0f * shape_.getScale().y)};
}

float EllipseShape::getOutlineThickness() const
{
    return shape_.getOutlineThickness();
}

const Color& EllipseShape::getOutlineColor() const
{
    return reinterpret_cast<const Color&>(shape_.getOutlineColor());
}

const IImage *EllipseShape::getImage() const
{
    if (imageNeedsUpdate_)
        updateImage();

    return cachedImage_.get();
}

void EllipseShape::move(const vec2f& offset)
{
    shape_.move(sf::Vector2f(offset.x, offset.y));
    imageNeedsUpdate_ = true;
}

void EllipseShape::updateImage() const
{
    sf::RenderTexture renderTexture;
    
    sf::FloatRect shapeBounds = shape_.getGlobalBounds();
    
    vec2u totalSize = { static_cast<unsigned>(shapeBounds.width  + shapeBounds.left + 1), 
                        static_cast<unsigned>(shapeBounds.height + shapeBounds.top  + 1) };

    bool createResult = renderTexture.create(totalSize.x, totalSize.y);
    assert(createResult);

    renderTexture.clear(sf::Color::Transparent);
    renderTexture.draw(shape_);
    renderTexture.display();

    sf::Image image = renderTexture.getTexture().copyToImage();

    cachedImage_ = std::make_unique<Image>();
    cachedImage_->create(image.getSize().x, image.getSize().y,
                         reinterpret_cast<const Color*>(image.getPixelsPtr()));

    cachedImage_->setPos(vec2i{0, 0});

    imageNeedsUpdate_ = false;
}

std::unique_ptr<IEllipseShape> IEllipseShape::create(unsigned int width, unsigned int height)
{
    return std::make_unique<EllipseShape>(width, height);    
}

std::unique_ptr<IEllipseShape> IEllipseShape::create(unsigned int radius)
{
    return std::make_unique<EllipseShape>(radius);
}

std::unique_ptr<IEllipseShape> IEllipseShape::create(const psapi::sfm::vec2u& size)
{
    return std::make_unique<EllipseShape>(size);
}

} // namespace sfm

} // namespace psapi
