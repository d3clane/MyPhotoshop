#ifndef PHOTOSHOP_PLUGIN_TOOLBAR_H
#define PHOTOSHOP_PLUGIN_TOOLBAR_H

#include "ps_bar.h"

extern "C" {
    bool   loadPlugin();
    void unloadPlugin();
}

namespace ps {

using psapi::sfm::vec2i;
using psapi::sfm::vec2f;

class Toolbar : public ABar {
public:
    Toolbar(vec2i pos, vec2i size);
    ~Toolbar() = default;

    void draw(psapi::ARenderWindow* renderWindow) override;
    bool update(const psapi::ARenderWindow* renderWindow,
                const psapi::sfm::Event& event) override;

    virtual psapi::ChildInfo getNextChildInfo() const override;

    virtual void finishButtonDraw(psapi::ARenderWindow* renderWindow, const psapi::IBarButton* button) override; 
    
    void addWindow(std::unique_ptr<IWindow> window) override;
    void removeWindow(psapi::wid_t id) override;

private:
    int gap_size = 16;

    mutable int next_child_index_ = 0;
    mutable int num_children_ = 0;

    vec2i child_size_ = {64, 64};
};

} // namespace

#endif // PHOTOSHOP_PLUGIN_TOOLBAR_H
