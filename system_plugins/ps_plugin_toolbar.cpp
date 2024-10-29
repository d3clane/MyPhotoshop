#include "ps_plugin_toolbar.h"

#include <cassert>
#include <iostream>
#include <memory>

extern "C" {
    
bool loadPlugin() {
    psapi::vec2i pos  = {0,   0  };
    psapi::vec2i size = {100, 600};
    auto toolbar = std::make_unique<ps::Toolbar>(pos, size);
    std::cerr << "TB ID - " << toolbar.get()->getId() << "\n";

    psapi::getRootWindow()->addWindow(std::move(toolbar));

    return true;
}

void unloadPlugin() {
}

}

using namespace ps;

Toolbar::Toolbar(vec2i pos, vec2i size) {
    pos_ = pos;
    size_ = size;

    texture_.loadFromFile("gray.png");
    sprite_.setTexture(&texture_);

    auto spriteSize = sprite_.getSize();

    sprite_.setScale(static_cast<double>(size_.x) / spriteSize.x, static_cast<double>(size_.y) / spriteSize.y);
    sprite_.setPosition(pos_.x, pos_.y);
}

void Toolbar::draw(psapi::ARenderWindow* renderWindow) {
    renderWindow->draw(&sprite_);

    //std::cerr << "Toolbar::draw" << std::endl;

    drawChildren(renderWindow);
}

bool Toolbar::update(const psapi::ARenderWindow* renderWindow,
                     const psapi::sfm::Event& event) {
    return updateChildren(renderWindow, event);
}

psapi::ChildInfo Toolbar::getNextChildInfo() const {
    vec2i pos = {gap_size, gap_size};
    pos.x += child_size_.x * next_child_index_;   

    return {pos, child_size_};
}

void Toolbar::finishButtonDraw(psapi::ARenderWindow* renderWindow, const psapi::IBarButton* button) {
}

void Toolbar::addWindow(std::unique_ptr<IWindow> window)
{
//TODO:
}

void Toolbar::removeWindow(psapi::wid_t id)
{
//TODO:
}