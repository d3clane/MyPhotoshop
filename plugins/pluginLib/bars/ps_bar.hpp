#ifndef API_IMPLEMENTATION_PS_BAR_H_
#define API_IMPLEMENTATION_PS_BAR_H_

#include "api/api_bar.hpp"
#include "api/api_sfm.hpp"
#include "pluginLib/windows/windows.hpp"

using namespace psapi;
using namespace sfm;

namespace ps
{

class ABar;

class ABarButton : public IBarButton
{
public:
    bool update(const IRenderWindow* renderWindow, const sfm::Event& event) override = 0;
    
    IWindow* getWindowById(wid_t id) override;
    const IWindow* getWindowById(wid_t id) const override;

    vec2i getPos()  const override;
    vec2u getSize() const override;
    wid_t getId()   const override;

    void forceDeactivate()                override;
    void forceActivate()                  override; 

    bool isActive() const override;
    bool isWindowContainer() const override;

    void setState(State state) override;
    State getState() const override;

    virtual void setPos (vec2i pos);
    virtual void setSize(vec2u size);

protected:
    bool updateState(const IRenderWindow* renderWindow, const Event& event);

protected:
    wid_t id_ = kInvalidWindowId;

    vec2i pos_;
    vec2u size_;

    bool isActive_ = true;

    State state_ = State::Normal;
};

class ASpritedBarButton : public ABarButton
{
public:
    void setPos (vec2i pos) override;
    void setSize(vec2u size) override;

    void draw(IRenderWindow* renderWindow) override = 0;

protected:
    void draw(IRenderWindow* renderWindow, const IBar* parent);

    std::unique_ptr<ISprite>  mainSprite_;
    std::unique_ptr<ITexture> mainTexture_;
};

class ANamedBarButton : public ABarButton
{
public:
    void setPos (vec2i pos) override;
    void setSize(vec2u size) override;

    void draw(IRenderWindow* renderWindow) override = 0;

protected:
    void draw(IRenderWindow* renderWindow, const IBar* parent);
    void rescaleNameToFit();  

protected:
    std::unique_ptr<IText> name_;
    std::shared_ptr<IFont> font_;
};

// TODO: add to instrument bar / color bar [ create PluginABars ]
class IPluginsBar : public IBar
{
    virtual size_t getNextChildId() const;
    virtual ChildInfo getChildInfo(size_t childId) const;
};

class ABar : public IBar
{
public:
    ~ABar();

    void draw(IRenderWindow* renderWindow) override;
    bool update(const IRenderWindow* renderWindow, const Event& event) override = 0;

    vec2i getPos()  const override;
    vec2u getSize() const override;
    wid_t getId()   const override;

    void forceDeactivate()                override;
    void forceActivate()                  override; 

    bool isActive() const override;
    bool isWindowContainer() const override;

protected:
    virtual void drawChildren(IRenderWindow* renderWindow) = 0;
    
    void setPos (vec2i pos);
    void setSize(vec2u size);

protected:
    wid_t id_ = kInvalidWindowId;

    bool isActive_ = true;

    vec2i pos_;
    vec2u size_;

    std::unique_ptr<IRectangleShape> shape_;
};

class APluginsBar : public IPluginsBar
{
    // TODO: 
};

class AShapedButtonsBar : public ABar
{
public:
    void finishButtonDraw(IRenderWindow* renderWindow, const IBarButton* button) const override;

protected:
    enum class SpriteType
    {
        Hover = 0,
        Press,
        Release,
        Count, // count of elements
    };

    void setShape(std::unique_ptr<IRectangleShape> shape, SpriteType pos);

protected:
    std::unique_ptr<IRectangleShape> commonOutlineShape_;
    std::unique_ptr<IRectangleShape> shapes_[static_cast<size_t>(SpriteType::Count)];
};

class AShapedButtonsPluginsBar : public ABar
{
    // TODO: 
};

namespace bar_children_handler_funcs
{

template<typename T>
bool updateChildren(const IRenderWindow* renderWindow, const Event& event, std::vector<T>& windowVector)
{
    bool updatedSomeone = false;

    static const size_t invalidPos = static_cast<size_t>(-1);
    size_t lastReleasedButtonPos = invalidPos;
    size_t windowsSize = windowVector.size();
    for (size_t i = 0; i < windowsSize; ++i)
    {
        IBarButton::State state = windowVector[i]->getState();
        updatedSomeone |= windowVector[i]->update(renderWindow, event);

        if (windowVector[i]->getState() == IBarButton::State::Released && state != IBarButton::State::Released)
            lastReleasedButtonPos = i;
    }

    if (lastReleasedButtonPos != invalidPos)
    {
        for (size_t i = 0; i < windowsSize; ++i)
        {
            if (i == lastReleasedButtonPos)
                continue;
            windowVector[i]->setState(IBarButton::State::Normal);
        }
    }

    return updatedSomeone;
}

} // namespace bar_children_handler_funcs

} // namespace ps

#endif // API_IMPLEMENTATION_PS_BAR_H_
