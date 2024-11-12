#ifndef PLUGINS_PLUGIN_LIB_INSTRUMENTS_BAR_HPP
#define PLUGINS_PLUGIN_LIB_INSTRUMENTS_BAR_HPP

#include "bars/ps_bar.hpp"
#include "windows/windows.hpp"

namespace ps
{

static const wid_t kInstrumentsBarId = 10001;

class InstrumentsBar;

class AInstrumentButton : public IBarButton, public AWindow
{
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

    void setPos (vec2i pos);
    void setSize(vec2u size);

protected:
    wid_t id_ = kInvalidWindowId;

    const InstrumentsBar* parent_ = nullptr;
    bool isActive_ = true;

    vec2i pos_;
    vec2u size_;

    std::unique_ptr<ISprite>  mainSprite_;
    std::unique_ptr<ITexture> mainTexture_;

    State state_ = State::Normal;

protected:
    bool updateState(const IRenderWindow* renderWindow, const Event& event);
};

class InstrumentsBar : public IBar
{
public:
    void draw(IRenderWindow* renderWindow) override;
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

protected:
    wid_t id_ = kInvalidWindowId;

    const IWindow* parent_ = nullptr;
    bool isActive_ = true;

    vec2i pos_;
    vec2u size_;

    std::unique_ptr<IRectangleShape> shape_;
    
    std::vector<std::unique_ptr<AWindow>> windows_;

protected:
    void   drawChildren(IRenderWindow* renderWindow);
    bool updateChildren(const IRenderWindow* renderWindow, const sfm::Event& event);

    void setPos (vec2i pos);
    void setSize(vec2u size);

private:
    enum class SpriteType
    {
        Hover = 0,
        Press,
        Release,
        Count, // count of elements
    };

public:
    InstrumentsBar(vec2i pos, vec2u size);

    void addWindow(std::unique_ptr<IWindow> window) override;
    void removeWindow(wid_t id) override;

    ChildInfo getNextChildInfo() const override;

    bool update(const IRenderWindow* renderWindow, const sfm::Event& event) override;

protected:
    void finishButtonDraw(IRenderWindow* renderWindow, const IBarButton* button) const override;

private:
    size_t gapSize_ = 16;

    std::unique_ptr<IRectangleShape> commonOutlineShape_;
    std::unique_ptr<IRectangleShape> shapes_[static_cast<size_t>(SpriteType::Count)];
}; 

} // namespace ps

#endif // PLUGINS_PLUGIN_LIB_INSTRUMENTS_BAR_HPP