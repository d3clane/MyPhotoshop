#ifndef API_IMPLEMENTATION_PS_BAR_H_
#define API_IMPLEMENTATION_PS_BAR_H_

#include "api/api_bar.hpp"
#include "api/api_sfm.hpp"
#include "pluginLib/windows/windows.hpp"
#include "pluginLib/sfmHelpful/sfmHelpful.hpp"

using namespace psapi;
using namespace sfm;

namespace ps
{

class ABar;

class ABarButton : public IBarButton
{
public:    
    IWindow* getWindowById(wid_t id) override;
    const IWindow* getWindowById(wid_t id) const override;

    vec2i getPos()  const override;
    vec2u getSize() const override;
    void setPos (const vec2i& pos ) override;
    void setSize(const vec2u& size) override;
    void setParent(const IWindow* parent) override;
    
    wid_t getId()   const override;

    void forceDeactivate() override;
    void forceActivate() override; 

    bool isActive() const override;
    bool isWindowContainer() const override;

    void setState(State state) override;
    State getState() const override;

protected:
    bool updateState(const IRenderWindow* renderWindow, const Event& event);

protected:
    wid_t id_ = kInvalidWindowId;

    vec2i pos_;
    vec2u size_;

    const IWindow* parent_ = nullptr;

    bool isActive_ = true;

    State state_ = State::Normal;
};

class ASpritedBarButton : public ABarButton
{
public:
    void setPos (const vec2i& pos) override;
    void setSize(const vec2u& size) override;

    void draw(IRenderWindow* renderWindow) override;

protected:
    std::unique_ptr<ISprite>  mainSprite_;
    std::unique_ptr<ITexture> mainTexture_;
};

class ANamedBarButton : public ABarButton
{
public:
    void setPos (const vec2i& pos) override;
    void setSize(const vec2u& size) override;

    void draw(IRenderWindow* renderWindow) override;

protected:
    std::unique_ptr<IText> name_;
    std::unique_ptr<IFont> font_;
};

class AMenuBarButton : public IMenuButton
{
public:
    vec2i getPos()  const override;
    vec2u getSize() const override;
    void setPos (const vec2i& pos ) override;
    void setSize(const vec2u& size) override;
    void setParent(const IWindow* parent) override;
    
    wid_t getId()   const override;

    void forceDeactivate()                override;
    void forceActivate()                  override; 

    bool isActive() const override;
    bool isWindowContainer() const override;

    void setState(State state) override;
    State getState() const override;

    void draw(IRenderWindow* renderWindow) override;

    vec2u getTextSize() const;

protected:
    bool updateState(const IRenderWindow* renderWindow, const Event& event);

protected:
    wid_t id_ = kInvalidWindowId;

    vec2i pos_;
    vec2u size_;

    const IWindow* parent_ = nullptr;

    bool isActive_ = true;

    State state_ = State::Normal;

    std::unique_ptr<IText> name_;
    std::unique_ptr<IFont> font_;
};

// TODO: add to instrument bar / color bar [ create PluginABars ]

class ABar : public IBar
{
public:
    ~ABar();

    void draw(IRenderWindow* renderWindow) override;

    vec2i getPos()  const override;
    vec2u getSize() const override;
    wid_t getId()   const override;

    void forceDeactivate()                override;
    void forceActivate()                  override; 

    bool isActive() const override;
    bool isWindowContainer() const override;

    void setPos (const vec2i& pos)  override;
    void setSize(const vec2u& size) override;
    void setParent(const IWindow* parent) override;
    
protected:
    vec2i calculateMiddleForChild(vec2u childSize);
    virtual void drawChildren(IRenderWindow* renderWindow) = 0;
    
protected:
    wid_t id_ = kInvalidWindowId;

    bool isActive_ = true;

    vec2i pos_;
    vec2u size_;
    const IWindow* parent_;

    std::unique_ptr<IRectangleShape> shape_;
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

class ASpritedButtonsBar : public ABar
{
public:
    ASpritedButtonsBar(vec2i pos, vec2u size, wid_t id, 
                       Color barColor,
                       const char* hoverSpriteFilename, 
                       const char* pressSpriteFilename,
                       const char* releaseSpriteFilename);

    void finishButtonDraw(IRenderWindow* renderWindow, const IBarButton* button) const override;

protected:
    enum class SpriteType
    {
        Hover = 0,
        Press,
        Release,
        Count, // count of elements
    };

    void setSprite(SpriteInfo spriteInfo, SpriteType pos);

protected:
    SpriteInfo sprites_[static_cast<size_t>(SpriteType::Count)];
};

template<typename T>
class BarUpdateChildrenAction : public IAction
{
public:
    BarUpdateChildrenAction(
        const IRenderWindow* renderWindow, const Event& event,
        std::vector<T*>& children) 
        : renderWindow_(renderWindow), event_(event), children_(children) {};

    bool execute(const Key& key) override;
    bool isUndoable(const Key& /* key */) override { return false; }

private:
    const IRenderWindow* renderWindow_;
    Event event_;

    std::vector<T*> children_;
};

template<typename T>
bool BarUpdateChildrenAction<T>::execute(const Key& /* key */)
{
    AActionController* actionController = getActionController();
    bool updatedSomeone = false;

    static const size_t invalidPos = static_cast<size_t>(-1);
    size_t lastReleasedButtonPos = invalidPos;
    size_t childrenSize = children_.size();
    for (size_t i = 0; i < childrenSize; ++i)
    {
        IBarButton::State state = children_[i]->getState();
        updatedSomeone |= actionController->execute(children_[i]->createAction(renderWindow_, event_));

        if (children_[i]->getState() == IBarButton::State::Released && 
            state != IBarButton::State::Released)
        {
            lastReleasedButtonPos = i;
        }
    }

    if (lastReleasedButtonPos != invalidPos)
    {
        for (size_t i = 0; i < childrenSize; ++i)
        {
            if (i == lastReleasedButtonPos)
                continue;
            children_[i]->setState(IBarButton::State::Normal);
        }
    }

    return updatedSomeone;
}

namespace bar_children_handler_funcs
{

template<typename T>
std::unique_ptr<IAction> createUpdateChildrenAction(const IRenderWindow* renderWindow, const Event& event,
                                                    std::vector<std::unique_ptr<T>>& windowVector)
{
    std::vector<T*> windowVectorPtrs;
    for (auto& window : windowVector)
        windowVectorPtrs.push_back(window.get());
    return std::make_unique<BarUpdateChildrenAction<T>>(renderWindow, event, windowVectorPtrs);
}

template<typename T>
bool unPressAllButtons(std::vector<T>& buttons)
{
    for (auto& button : buttons)
        button->setState(IBarButton::State::Normal);

    return true;
}


} // namespace bar_children_handler_funcs

} // namespace ps

#endif // API_IMPLEMENTATION_PS_BAR_H_
