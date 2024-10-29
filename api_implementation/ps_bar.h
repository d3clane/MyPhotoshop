#ifndef API_IMPLEMENTATION_PS_BAR_H_
#define API_IMPLEMENTATION_PS_BAR_H_

#include "api_bar.hpp"
#include "sfm_implementation.h"

class ABarButton : public psapi::IBarButton {
public: 
    virtual void draw(psapi::ARenderWindow* renderWindow)         override;
    virtual bool update(const psapi::ARenderWindow* renderWindow,
                        const psapi::sfm::Event& event)           override = 0;

    virtual       psapi::IWindow* getWindowById(psapi::wid_t id)       override;
    virtual const psapi::IWindow* getWindowById(psapi::wid_t id) const override;

    virtual psapi::vec2i getPos()  const override;
    virtual psapi::vec2i getSize() const override;
    virtual psapi::wid_t getId()   const override;

    virtual void setParent(const IWindow* parent) override;
    virtual void forceDeactivate()                override;
    virtual void forceActivate()                  override; 

protected:
    psapi::wid_t id_ = psapi::kInvalidWindowId;

    const psapi::IWindow* parent_ = nullptr;
    bool is_active_ = true;

    psapi::vec2i pos_;
    psapi::vec2i size_;

    psapi::sfm::Sprite sprite_;
};

class ABar : public psapi::IBar {
public:
    virtual ~ABar();

    virtual       psapi::IWindow* getWindowById(psapi::wid_t id)       override;
    virtual const psapi::IWindow* getWindowById(psapi::wid_t id) const override;

    virtual psapi::vec2i getPos()  const override;
    virtual psapi::vec2i getSize() const override;
    virtual psapi::wid_t getId()   const override;

    virtual void setParent(const IWindow* parent) override;
    virtual void forceDeactivate()                override;
    virtual void forceActivate()                  override; 

protected:
    psapi::wid_t id_ = psapi::kToolBarWindowId;

    const psapi::IWindow* parent_ = nullptr;
    bool is_active_ = true;

    psapi::vec2i pos_;
    psapi::vec2i size_;

    psapi::sfm::Sprite sprite_;
    psapi::sfm::Texture texture_;

    std::vector<std::unique_ptr<IWindow> > windows_;
    void   drawChildren(psapi::ARenderWindow* renderWindow);
    bool updateChildren(const psapi::ARenderWindow* renderWindow,
                        const psapi::sfm::Event& event);
};

#endif // API_IMPLEMENTATION_PS_BAR_H_
