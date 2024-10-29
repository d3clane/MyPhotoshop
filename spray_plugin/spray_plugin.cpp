#include "spray_plugin.h"
#include <string>

#include "photoshopApi/api_sfm.hpp"
#include "photoshopAPI/api_photoshop.hpp"
#include "photoshopAPI/api_bar.hpp"
#include "photoshopAPI/api_canvas.hpp"

#include <iostream>

using namespace psapi;
using namespace psapi::sfm;

class SprayButton : public IBarButton 
{
public:
    SprayButton() = default;
    virtual void draw(psapi::ARenderWindow* renderWindow)         override;
    virtual bool update(const psapi::ARenderWindow* renderWindow,
                        const psapi::sfm::Event& event)           override;

    virtual       psapi::IWindow* getWindowById(psapi::wid_t id)       override;
    virtual const psapi::IWindow* getWindowById(psapi::wid_t id) const override;

    virtual psapi::vec2i getPos()  const override;
    virtual psapi::vec2i getSize() const override;
    virtual psapi::wid_t getId()   const override;

    virtual void setParent(const IWindow* parent) override;
    virtual void forceDeactivate()                override;
    virtual void forceActivate()                  override; 

    bool isWindowContainer() const override { return false; }

    State getState() const override { return state_; }
    void setState(State state) { /*TODO*/ }

public: // TODO: change, just for now
    psapi::wid_t id_ = psapi::kInvalidWindowId;

    const psapi::IWindow* parent_ = nullptr;
    bool is_active_ = true;

    psapi::vec2i pos_;
    psapi::vec2i size_;

    std::unique_ptr<ASprite> sprite_;

    State state_;
};


void SprayButton::draw(psapi::ARenderWindow* renderWindow) {
    renderWindow->draw(sprite_.get());
}

bool SprayButton::update(const psapi::ARenderWindow* renderWindow,
                         const psapi::sfm::Event& event)
{
    vec2i mousePosAbs = Mouse::getPosition(renderWindow);

    bool notHovered = (mousePosAbs.x < pos_.x || mousePosAbs.x > pos_.x + size_.x
        || mousePosAbs.y < pos_.y || mousePosAbs.y > pos_.y + size_.y);
    std::cerr << "NOT HOVERED? " << notHovered << "\n";

    if (event.type == event.MouseButtonReleased && !notHovered)
    {
        std::cout << "STATE\n";
        if (state_ == State::Normal)
        {
            state_ = State::Released;
            std::cout << "SPRAY BUTTON TURN ON\n";
        }
        else
        {
            state_ = State::Normal;
            std::cout << "SPRAY BUTTON TURN OFF\n";
        }
    }

    if (event.type == event.MouseButtonPressed)
        std::cerr << "PRESSED\n";
    if (state_ != State::Released || event.type != event.MouseButtonPressed)
        return true;
    
    std::cerr << "HERE\n";
    ICanvas* w = static_cast<ICanvas*>(getRootWindow()->getWindowById(kCanvasWindowId));

    if (!w->isPressed())
        return true;

    if (!w) {
        std::cerr << "NOT FOUND???\n";
        abort();
    }

    std::cerr << "VV\n";
    assert(w->getLayer(0));
    
    auto mousePos = w->getMousePosition();
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            w->getLayer(0)->setPixel({mousePos.x + i, mousePos.y + j}, {0xFF, 0x00, 0x00, 0xFF});
    
    std::cerr << "mousePos.x, mousePos.y: " << mousePos.x << ", " << mousePos.y << std::endl;

    std::cerr << "ZZ\n";
    return false;
}

IWindow* SprayButton::getWindowById(psapi::wid_t id) {
    if (id == id_) {
        return this;
    }

    return nullptr;
}

const IWindow* SprayButton::getWindowById(psapi::wid_t id) const {
    if (id == id_) {
        return this;
    }

    return nullptr;
}

psapi::vec2i SprayButton::getPos() const {
    return pos_;
}

psapi::vec2i SprayButton::getSize() const {
    return size_;
}

psapi::wid_t SprayButton::getId() const {
    return id_;
}

void SprayButton::setParent(const psapi::IWindow* parent) {
    parent_ = parent;
}

void SprayButton::forceDeactivate() {
    is_active_ = false;
}

void SprayButton::forceActivate() {
    is_active_ = true;
}

bool loadPlugin1()
{
    auto buttonSprite = std::unique_ptr<ASprite>(ASprite::create());
    auto buttonTexture = std::unique_ptr<ATexture>(ATexture::create());

    buttonTexture.get()->loadFromFile("spray.png");

    buttonSprite->setTexture(buttonTexture.get());

    IWindowContainer* rootWindow = getRootWindow();
    auto toolBar = static_cast<IBar*>(rootWindow->getWindowById(kToolBarWindowId));
    assert(toolBar);

    auto info = toolBar->getNextChildInfo();
    auto pos = info.pos;
    auto size = info.size;

    buttonSprite->setPosition(pos.x, pos.y);
    std::cout << buttonSprite.get()->getSize().x << " " << buttonSprite.get()->getSize().y << std::endl;
    
    auto spriteSize = buttonSprite->getSize();
    buttonSprite->setScale(static_cast<double>(size.x) / spriteSize.x, static_cast<double>(size.y) / spriteSize.y);

    auto button = std::make_unique<SprayButton>();
    button->sprite_ = std::move(buttonSprite);

    button->pos_ = pos;
    button->size_ = size;

    rootWindow->addWindow(std::move(button)); // TODO: ADD TO TOOLBAR!!

    std::cerr << "ME\n";
    buttonTexture.release(); // mem leak

    std::cerr << "ME\n\n";
    return true;
}

void unloadPlugin1()
{
    return;
}