#ifndef API_IMPLEMENTATION_PS_BAR_H_
#define API_IMPLEMENTATION_PS_BAR_H_

#include "api/api_bar.hpp"
#include "api/api_sfm.hpp"

using namespace psapi;
using namespace sfm;

namespace ps
{

class ABarButton : public IBarButton {
public: 
    void draw(IRenderWindow* renderWindow)         override;
    bool update(const IRenderWindow* renderWindow, const sfm::Event& event) override = 0;

    IWindow* getWindowById(wid_t id) override;
    const IWindow* getWindowById(wid_t id) const override;

    vec2i getPos()  const override;
    vec2u getSize() const override;
    wid_t getId()   const override;

    void setParent(const IWindow* parent) override;
    void forceDeactivate()                override;
    void forceActivate()                  override; 

    bool isActive() const override;
    bool isWindowContainer() const override;

    void setState(State state) override;
    State getState() const override;
protected:
    wid_t id_ = kInvalidWindowId;

    const IWindow* parent_ = nullptr;
    bool isActive_ = true;

    vec2i pos_;
    vec2u size_;

    std::unique_ptr<ISprite>  mainSprite_;
    std::shared_ptr<ITexture> mainTexture_;

    State state_;
};

class ABar : public IBar {
public:
    ~ABar();

    void draw(IRenderWindow* renderWindow) override;
    bool update(const IRenderWindow* renderWindow, const sfm::Event& event) override;

    IWindow* getWindowById(wid_t id) override;
    const IWindow* getWindowById(wid_t id) const override;

    vec2i getPos()  const override;
    vec2u getSize() const override;
    wid_t getId()   const override;

    void setParent(const IWindow* parent) override;
    void forceDeactivate()                override;
    void forceActivate()                  override; 

    bool isActive() const override;
    bool isWindowContainer() const override;
protected:
    wid_t id_ = kToolBarWindowId;

    const IWindow* parent_ = nullptr;
    bool isActive_ = true;

    vec2i pos_;
    vec2u size_;

    std::unique_ptr<ISprite> sprite_;
    std::shared_ptr<ITexture> texture_;

    std::vector<std::unique_ptr<ABarButton> > windows_;

protected:
    void   drawChildren(IRenderWindow* renderWindow);
    bool updateChildren(const IRenderWindow* renderWindow, const sfm::Event& event);
};

} // namespace ps

#endif // API_IMPLEMENTATION_PS_BAR_H_
