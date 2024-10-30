#ifndef SFM_IMPLEMENTATION_HPP
#define SFM_IMPLEMENTATION_HPP

#include "api/api_sfm.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>

namespace psapi
{

namespace sfm
{

class RenderWindow : public IRenderWindow 
{
private:
    sf::RenderWindow window_;

    friend class Text;
    friend class Sprite;
    friend class Mouse;

public:
    RenderWindow(unsigned int width, unsigned int height, const std::string& title);

    bool isOpen() const override;
    void clear()   override;
    void display() override;
    void close()   override;

    vec2u getSize() const override;

    bool pollEvent(Event& event) override;

    void draw(Drawable *target) override;

    void setFps(float fps) override;
    float getFps() const override;
};

class Texture : public ITexture 
{
private:
    sf::Texture texture_;

    friend class Sprite;

public:
    bool create(unsigned int width, unsigned int height)                                     override;
    bool loadFromFile  (const std::string& filename,        const IntRect& area = IntRect()) override;
    bool loadFromMemory(const void* data, std::size_t size, const IntRect& area = IntRect()) override;
    vec2u getSize() const                                                                    override;
    std::unique_ptr<IImage> copyToImage() const                                              override;
    void update(const IImage *image)                                                         override;
    void update(const Color *pixels)                                                         override;
    void update(const Color *pixels, unsigned int width, unsigned int height,
                                             unsigned int x,     unsigned int y) override;
};

class Sprite : public ISprite 
{
private:
    sf::Sprite sprite_;

public:
    ~Sprite() = default;

    void setTexture(const ITexture *texture, bool reset_rect = false) override;
    void setTextureRect(const IntRect &rectangle) override;

    void setPosition(float x, float y) override;
    void setPosition(const vec2f &pos) override;

    void setScale(float factorX, float factorY) override;
    vec2u getSize() const override;

    void setColor(const Color &color) override;
    Color getColor() const override;

    void setRotation(float angle) override;

    const vec2f getPosition() const override;
    IntRect getGlobalBounds() const override;

    void draw(IRenderWindow *window) const override;
};

} // namespace sfm
} // namespace psapi

#endif // SFM_IMPLEMENTATION_HPP

