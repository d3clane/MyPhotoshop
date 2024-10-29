#include "ps_windows.h"
#include <cassert>
#include <iostream>

using namespace ps;

int64_t AWindow::getId() const {
    return id_;
}

psapi::IWindow* AWindow::getWindowById(psapi::wid_t id) {
    if (id == id_) {
        return this;
    }
    return nullptr;
}

const psapi::IWindow* AWindow::getWindowById(psapi::wid_t id) const {
    if (id == id_) {
        return this;
    }
    return nullptr;
}

// AWindowContainer implementation 

int64_t AWindowContainer::getId() const {
    return id_;
}

// AWindowVector implementation

void AWindowVector::drawChildren(IRenderWindow* renderWindow) {
    for (auto& window : windows_) {
        window->draw(renderWindow);
    }
}

bool AWindowVector::updateChildren(const psapi::ARenderWindow* renderWindow,
                                   const psapi::sfm::Event& event) {
    bool was_updated = false;
    for (auto& window : windows_) {
        was_updated |= window->update(renderWindow, event);
    }
    return was_updated;
}

int64_t AWindowVector::getId() const {
    return id_;
}

psapi::IWindow* AWindowVector::getWindowById(psapi::wid_t id) {
    return const_cast<psapi::IWindow*>(static_cast<const AWindowVector*>(this)->getWindowById(id));
}

const psapi::IWindow* AWindowVector::getWindowById(psapi::wid_t id) const {
    if (id == id_) {
        return this;
    }

    for (const auto& window : windows_) {
        psapi::IWindow* result = window->getWindowById(id);
        if (result) {
            return result;
        }
    }

    return nullptr;
}

void AWindowVector::assertDuplicateId(psapi::wid_t id) {
    if (id == psapi::kInvalidWindowId) {
        return;
    }

    for (const auto& window : windows_) {
        if (window->getId() == id) {
            assert(0);
        }
    }
}

void AWindowVector::addWindow(std::unique_ptr<IWindow> window) {
    assertDuplicateId(window->getId());

    std::cerr << "PUSHING WINDOW WITH ID - " << window->getId() << "\n";
    windows_.push_back(std::move(window));
}

void AWindowVector::removeWindow(psapi::wid_t id) {
    for (auto it = windows_.begin(); it != windows_.end(); it++) {
        if ((*it)->getId() == id) {
            windows_.erase(it);
            return;
        }
    }
}

// 

using namespace psapi;

// getRootwindow singleton implementation

namespace psapi {
    
IWindowContainer* getRootWindow() {
    static RootWindow rootWindow;
    return &rootWindow;
}

generalFunction getGeneralFunction(const std::string& name) {
    return nullptr;
}

} // namespace
