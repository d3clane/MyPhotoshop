#include "filterWindows.hpp"

#include "pluginLib/actions/actions.hpp"
#include "slider.hpp"

#include <string>
#include <cassert>

namespace ps
{

static const vec2u RenderWindowSize = {800, 600};

FilterWindow::FilterWindow(wid_t id, const char* filterWindowName)
    : id_(id)
{
    renderWindow_ = IRenderWindow::create(RenderWindowSize.x, RenderWindowSize.y, filterWindowName);
}


void FilterWindow::draw(IRenderWindow* /* renderWindow */)
{ 
    if (!isActive_)
        return;

    renderWindow_->clear();

    for (auto& window : windows_)
    {
        if (!window)
            continue;
            
        window->draw(renderWindow_.get());
    }

    renderWindow_->display();
}

std::unique_ptr<IAction> FilterWindow::createAction(const IRenderWindow* /* renderWindow */, 
                                                    const Event& event)
{
    return std::make_unique<UpdateCallbackAction<FilterWindow>>(*this, renderWindow_.get(), event);
}

bool FilterWindow::update(const IRenderWindow* /* renderWindow */, const Event& /* event */)
{
    if (!isActive_)
    {
        fprintf(stderr, "INACTIVE\n");
        return false;
    }
    // TODO: bad stuff... Let's just believe that no actions would be undoable and no bugs are here

    AActionController* actionController = getActionController();

    // TODO: govnokod -> create class for root window to call execute for it, not to run through children

    if (!renderWindow_->isOpen())
        return false;

    Event event;

    while (renderWindow_->pollEvent(event))
    {
        if (event.type == psapi::sfm::Event::Closed)
        {
            renderWindow_->close();
            return false;
        }

        for (auto& window : windows_)
            actionController->execute(window->createAction(renderWindow_.get(), event));
    }

    if (event.type == Event::None)
    {
        for (auto& window : windows_)
            actionController->execute(window->createAction(renderWindow_.get(), event));
    }

    return true;
}

wid_t FilterWindow::getId() const
{
    return id_;
}

IWindow* FilterWindow::getWindowById(wid_t id)
{
    if (id == id_)
        return this;

    for (auto& window : windows_)
    {
        if (!window)
            continue;

        IWindow* searchRes = window->getWindowById(id);
        if (searchRes)
            return searchRes;
    }

    return nullptr;
}

const IWindow* FilterWindow::getWindowById(wid_t id) const
{
    return const_cast<FilterWindow*>(this)->getWindowById(id);
}

vec2i FilterWindow::getPos() const
{
    return {0, 0};
}

vec2u FilterWindow::getSize() const
{
    return renderWindow_->getSize();
}

void FilterWindow::setSize(const vec2u& /* size */)
{
    assert(0 && "CAN'T SET SIZE TO FILTER WINDOW");
}

void FilterWindow::setPos(const vec2i& /* pos */)
{
    assert(0 && "CAN'T SET POS TO FILTER WINDOW");
}

void FilterWindow::setParent(const IWindow* /* parent */)
{
    assert(0 && "CAN'T SET PARENT TO FILTER WINDOW");
}

void FilterWindow::forceActivate()
{
    isActive_ = true;
}

void FilterWindow::forceDeactivate()
{
    isActive_ = false;
}

bool FilterWindow::isActive() const
{
    return isActive_;
}

bool FilterWindow::isWindowContainer() const
{
    return true;
}

void FilterWindow::addWindow(std::unique_ptr<IWindow> window)
{
    windows_.push_back(std::move(window));
}

void FilterWindow::removeWindow(wid_t id)
{
    for (auto it = windows_.begin(); it != windows_.end(); ++it)
    {
        if ((*it)->getId() == id)
        {
            windows_.erase(it);
            return;
        }
    }
}

bool FilterWindow::closed() const
{
    return renderWindow_->isOpen();
}

void FilterWindow::close()
{
    renderWindow_->close();
    renderWindow_.reset();
}

// functions

namespace 
{

class SliderTitleAction : public ISliderTitleAction
{
public:
    SliderTitleAction(NamedSlider* slider) : slider_(slider) {}
    std::string getSliderTitle() const override;

private:
    NamedSlider* slider_;
};

std::string SliderTitleAction::getSliderTitle() const
{
    float thickness = slider_->getCurrentValue();

    return std::to_string(static_cast<int>(thickness));
}

} // namespace anonymous

std::unique_ptr<FilterWindow> createSimpleFilterWindow(const char* name)
{
    auto filterWindow = std::make_unique<FilterWindow>(kInvalidWindowId, name);

    SpriteInfo slideNormal = createSprite("media/textures/longSliderNormal.png");
    SpriteInfo slidePress  = createSprite("media/textures/longSliderPress.png");

    assert(slideNormal.sprite->getSize().x == slidePress.sprite->getSize().x);
    assert(slideNormal.sprite->getSize().y == slidePress.sprite->getSize().y);

    Color blueSliderColor = Color{74, 115, 145, 255};
    vec2u spritesOutlineWidth = {3, 2};
    vec2u maxFillColorSize = slideNormal.sprite->getSize() - 2 * spritesOutlineWidth;
    maxFillColorSize.x = 432;

    const float maxRadius = 100;
    auto namedSlider = std::make_unique<NamedSlider>("Radius: ", maxRadius);

    auto slider = std::make_unique<SliderX>(
                    vec2i{0, 0}, slideNormal.sprite->getSize(), kInvalidWindowId, 
                    createShape(blueSliderColor, {1, 1}), 
                    std::move(slideNormal), std::move(slidePress),
                    maxFillColorSize, spritesOutlineWidth,
                    std::make_unique<SliderTitleAction>(namedSlider.get()));

    namedSlider->setSlider(std::move(slider));
    namedSlider->setPos({100, 100});

    auto emptyWindow = std::make_unique<EmptyWindow>(createSprite("media/textures/renderWindowColor.png"));
    emptyWindow->setSize(RenderWindowSize);
    
    filterWindow->addWindow(std::move(emptyWindow)); // order is important
    filterWindow->addWindow(std::move(namedSlider));

    fprintf(stderr, "FILTER WINDOW IS CREATED\n");

    return filterWindow;
}

} // namespace ps