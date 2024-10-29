#include "ps_bar.h"


#include <cassert>

void ABarButton::draw(psapi::ARenderWindow* renderWindow) {
    //renderWindow->draw(&sprite_);
}

psapi::IWindow* ABarButton::getWindowById(psapi::wid_t id) {
    if (id == id_) {
        return this;
    }

    return nullptr;
}

const psapi::IWindow* ABarButton::getWindowById(psapi::wid_t id) const {
    if (id == id_) {
        return this;
    }

    return nullptr;
}

psapi::vec2i ABarButton::getPos() const {
    return pos_;
}

psapi::vec2i ABarButton::getSize() const {
    return size_;
}

psapi::wid_t ABarButton::getId() const {
    return id_;
}

void ABarButton::setParent(const psapi::IWindow* parent) {
    parent_ = parent;
}

void ABarButton::forceDeactivate() {
    is_active_ = false;
}

void ABarButton::forceActivate() {
    is_active_ = true;
}

// ABar implementation

void ABar::drawChildren(psapi::ARenderWindow* renderWindow) {
    for (const auto& window : windows_) {
        window->draw(renderWindow);
    }
}

bool ABar::updateChildren(const psapi::ARenderWindow* renderWindow,
                          const psapi::sfm::Event& event) {
    for (const auto& window : windows_) {
        if (window->update(renderWindow, event)) {
            return true;
        }
    }
    return false;
}

const psapi::IWindow* ABar::getWindowById(psapi::wid_t id) const {
    if (id_ == id)
        return this;
        
    for (auto& window : windows_) {
        if (window->getId() == id) {
            return window.get();
        }
    }
    return nullptr;
}

ABar::~ABar() {
}

psapi::IWindow* ABar::getWindowById(psapi::wid_t id) {
    return const_cast<psapi::IWindow*>(static_cast<const ABar*>(this)->getWindowById(id));
}

psapi::vec2i ABar::getPos()  const {
    return pos_;
}

psapi::vec2i ABar::getSize() const {
    return size_;
}

psapi::wid_t ABar::getId() const {
    return psapi::kToolBarWindowId;
}

void ABar::setParent(const IWindow* parent) {
    parent_ = parent;
}

void ABar::forceDeactivate() {
    is_active_ = false;
}

void ABar::forceActivate() {
    is_active_ = true;
}
