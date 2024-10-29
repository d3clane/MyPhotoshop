#ifndef API_IMPLEMENTATION_PS_WINDOW_H_
#define API_IMPLEMENTATION_PS_WINDOW_H_

#include "api/api_photoshop.hpp"

namespace ps {

using namespace psapi;
using namespace psapi::sfm;

class AWindow : public IWindow {
public:
    virtual ~AWindow() = default;

    virtual int64_t getId() const override;

    virtual       IWindow* getWindowById(wid_t id)       override;
    virtual const IWindow* getWindowById(wid_t id) const override;

protected:
    wid_t id_ = kInvalidWindowId;
};

} // namespace

class AWindowContainer : public IWindowContainer {
public:
    virtual int64_t getId() const override;

protected:
    wid_t id_ = kInvalidWindowId;
};

class AWindowVector : public IWindowVector {
public:
    virtual ~AWindowVector() = default;

    virtual int64_t getId() const override;
    virtual       IWindow* getWindowById(wid_t id)       override;
    virtual const IWindow* getWindowById(wid_t id) const override;

    virtual void addWindow(std::unique_ptr<IWindow> window) override;
    virtual void removeWindow(wid_t id)              override;

protected:
    void assertDuplicateId(wid_t id);
    void drawChildren(IRenderWindow* renderWindow);
    bool updateChildren(const IRenderWindow* renderWindow, const Event& event);


    wid_t id_ = kInvalidWindowId;
};

class RootWindow : public AWindowVector {
public:
    virtual void draw  (      IRenderWindow* renderWindow) override;
    virtual bool update(const IRenderWindow* renderWindow,
                        const Event& event)           override;
    virtual int64_t getId() const override;

    virtual vec2i getPos()  const          override;
    virtual vec2u getSize() const          override;
    virtual void setParent(const IWindow* parent) override;
    virtual void forceActivate()                  override;
    virtual void forceDeactivate()                override;

private:
    bool is_active_ = true;
};

#endif  // API_IMPLEMENTATION_PS_WINDOW_H_
