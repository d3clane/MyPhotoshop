#include "Graphics/PixelsArray.hpp"

namespace Graphics
{

namespace 
{

unsigned int getPixelPos(unsigned int x, unsigned int y, unsigned int width)
{
    return (y * width + x) * kBytesPerPixel;
}

uint8_t addColors(uint8_t col1, uint8_t col2, uint8_t maxColorValue)
{
    const uint16_t sumColors = (uint16_t)col1 + (uint16_t)col2;
    return sumColors > (uint16_t)maxColorValue ? maxColorValue : sumColors;
}

uint8_t mulColor(uint8_t col, double coeff, uint8_t maxColorValue)
{
    double val = col * coeff;

    return val > maxColorValue ? maxColorValue : val;
}

} // Anonymous namespace

Color Color::operator +(const Color& other) const
{
    return Color(addColors(red_  ,  other.red_  , maxColorValue_), 
                 addColors(green_,  other.green_, maxColorValue_),
                 addColors(blue_ ,  other.blue_ , maxColorValue_), 
                 std::max (alpha_,  other.alpha_));

}
Color Color::operator *(const double& coeff) const
{
    return Color(mulColor(red_  , coeff, maxColorValue_),
                 mulColor(green_, coeff, maxColorValue_),
                 mulColor(blue_ , coeff, maxColorValue_),
                 alpha_);
}

Color Color::operator *(const Color& other) const
{
    return Color(mulColor(red_  , (double)other.red_   / maxColorValue_, maxColorValue_),
                 mulColor(green_, (double)other.green_ / maxColorValue_, maxColorValue_),
                 mulColor(blue_ , (double)other.blue_  / maxColorValue_, maxColorValue_),
                 std::max(alpha_, other.alpha_));
}

PixelsArray::PixelsArray(
    unsigned int width, unsigned int height, 
    const Graphics::WindowPoint& topLeftPixelPos
    ) :  width_(width), height_(height), pixels_(new sf::Uint8[width * height * kBytesPerPixel]{0}), 
        topLeftPixelWindowPos_(topLeftPixelPos)
{
    for (unsigned int y = 0; y < height_; ++y)
    {
        for (unsigned int x = 0; x < width_; ++x)
        {
            Color defaultColor{};
            pixels_[getPixelPos(x, y, width_) + 0] = defaultColor.red_  ;
            pixels_[getPixelPos(x, y, width_) + 1] = defaultColor.green_;
            pixels_[getPixelPos(x, y, width_) + 2] = defaultColor.blue_ ;
            pixels_[getPixelPos(x, y, width_) + 3] = defaultColor.alpha_;
        }
    }
}

PixelsArray::PixelsArray(const PixelsArray& pixels) : 
    PixelsArray::PixelsArray(pixels.width_, pixels.height_, pixels.topLeftPixelWindowPos_)
{
    memcpy(pixels_, pixels.pixels_, width_ * height_ * kBytesPerPixel);
}

PixelsArray::~PixelsArray()
{
    delete[] pixels_;
}

PixelsArray& PixelsArray::operator =(PixelsArray& pixels)
{
    if (this == &pixels) return pixels;

    delete[] pixels_;

    width_  = pixels.width_;
    height_ = pixels.height_;
    memcpy(pixels_, pixels.pixels_, width_ * height_ * kBytesPerPixel);

    return *this;
}

void PixelsArray::setPixel(unsigned int x, unsigned int y, const Color& color)
{
    assert(x < width_ && y < height_);

    size_t pixelPos = getPixelPos(x, y, width_);

    pixels_[pixelPos + 0] = color.red_  ;
    pixels_[pixelPos + 1] = color.green_;
    pixels_[pixelPos + 2] = color.blue_ ;
    pixels_[pixelPos + 3] = color.alpha_;
}

Color PixelsArray::getPixel(unsigned int x, unsigned int y) const
{
    assert(x < width_ && y < height_);

    size_t pixelPos = getPixelPos(x, y, width_);

    return Color(pixels_[pixelPos + 0], pixels_[pixelPos + 1], pixels_[pixelPos + 2], pixels_[pixelPos + 3]);
}

// TODO: Copypaste
void PixelsArray::moveImageByX(int dx)
{
    if (dx == 0) return;

    if (dx > 0)
    {
        for (unsigned int y = 0; y < height_; ++y)
        {
            for (unsigned int x = width_ - 1; x >= dx; --x)
                setPixel(x, y, getPixel(x - dx, y));
            for (unsigned int x = std::min((unsigned int)dx - 1, width_ - 1); x + 1 > 0; --x)
                setPixel(x, y, Color());
        }
    }
    else
    {
        for (unsigned int y = 0; y < height_; ++y)
        {
            for (unsigned int x = 0; x - dx < width_; ++x)
                setPixel(x, y, getPixel(x - dx, y));
            for (unsigned int x = std::max(width_ + dx, 0u); x < width_; ++x)
                setPixel(x, y, Color());
        }
    }
}

void PixelsArray::moveImageByY(int dy)
{
    if (dy == 0) return;

    if (dy > 0)
    {
        for (unsigned int x = 0; x < width_; ++x)
        {
            for (unsigned int y = height_ - 1; y >= dy; --y)
                setPixel(x, y, getPixel(x, y - dy));
            for (unsigned int y = std::min((unsigned int)dy - 1, height_ - 1); y + 1 > 0; --y)
                setPixel(x, y, Color());
        }
    }
    else
    {
        for (unsigned int x = 0; x < width_; ++x)
        {
            for (unsigned int y = 0; y - dy < height_; ++y)
                setPixel(x, y, getPixel(x, y - dy));
            for (unsigned int y = std::max(height_ + dy, 0u); y < height_; ++y)
                setPixel(x, y, Color());
        }
    }
}

void PixelsArray::moveImage(Graphics::WindowVector direction)
{
    moveImageByX(direction.x);
    moveImageByY(direction.y);
}

void PixelsArray::draw(RenderWindow& window)
{
    window.drawPixels(*this);
}

/*
const PixelsArray& PixelsArray::operator =(const PixelsArray& pixels)
{
    if (this == &pixels) return pixels;

    delete[] pixels_;

    width_  = pixels.width_;
    height_ = pixels.height_;
    pixels_ = new sf::Uint8[width_ * height_ * kBytesPerPixel];
    memcpy(pixels_, pixels.pixels_, width_ * height_ * kBytesPerPixel);
}
*/

} // Graphics 