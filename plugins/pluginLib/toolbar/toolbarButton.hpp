#ifndef PLUGINS_PLUGIN_LIB_TOOLBAR_BUTTON_HPP
#define PLUGINS_PLUGIN_LIB_TOOLBAR_BUTTON_HPP

#include "api/api_bar.hpp"
#include "pluginLib/bars/ps_bar.hpp"
#include "pluginLib/instrumentBar/instrumentBar.hpp"

#include <cassert>

// TODO: mediator in instrument is non const, not ok
namespace ps
{

template<typename MediatorType>
class AInstrumentButton : public ASpritedBarButton
{
public:
    void setParent(const IWindow* parent) override;
    void setInstrumentBar(std::unique_ptr<IBar> instrumentBar);
    void setMediator(std::shared_ptr<MediatorType> mediator);

protected:
    const IBar* parent_;
    std::unique_ptr<IBar> instrumentBar_;

    std::shared_ptr<MediatorType> mediator_;
};

namespace instrument_button_functions
{

void updateInstrumentBarState(IBar* instrumentBar, IBarButton::State stateNow);

// This two functions have to be called on each update of the instrument button.
std::unique_ptr<IAction> createActionInstrumentBar(IBar* instrumentBar, IBarButton::State stateNow,
                                                   const IRenderWindow* renderWindow, const Event& event);

void drawInstrumentBar(IBar* instrumentBar, IRenderWindow* renderWindow);

template<typename ButtonType, typename MediatorType>
bool instrumentButtonOnLoadPlugin(const std::string& fileWithTextureName);

} // namespace instrument_button_functions 


// ------------------------------------implementation------------------------------------

// AInstrumentButton implementation
template<typename T>
void AInstrumentButton<T>::setMediator(std::shared_ptr<T> mediator)
{
    mediator_ = mediator;
}

template<typename T>
void AInstrumentButton<T>::setParent(const IWindow* parent)
{
    assert(parent);

    parent_ = dynamic_cast<const IBar*>(parent);
    assert(parent_);
}

template<typename T>
void AInstrumentButton<T>::setInstrumentBar(std::unique_ptr<IBar> instrumentBar)
{
    instrumentBar_ = std::move(instrumentBar);
    assert(instrumentBar_);
}

// loading plugin function implementation

namespace instrument_button_functions
{

namespace static_functions
{

template<typename ButtonType, typename MediatorType>
std::unique_ptr<ASpritedBarButton> createAInstrumentButton(std::shared_ptr<MediatorType> mediator,
                                                           const std::string& fileWithTextureName);

} // namespace static_functions

template<typename ButtonType, typename MediatorType>
bool instrumentButtonOnLoadPlugin(const std::string& fileWithTextureName)
{
    IWindowContainer* rootWindow = getRootWindow();
    assert(rootWindow);
    auto toolbar = static_cast<IBar*>(rootWindow->getWindowById(kToolBarWindowId));
    assert(toolbar);

    auto mediator = std::make_shared<MediatorType>();
    //auto instrumentBar = createCommonInstrumentBar(mediator);
    auto button = static_functions::createAInstrumentButton<ButtonType, MediatorType>(            
        mediator, fileWithTextureName
    );

    //assert(instrumentBar.get() == nullptr);

    toolbar->addWindow(std::move(button));

    return true;
}

template<typename ButtonType, typename MediatorType>
std::unique_ptr<ASpritedBarButton> static_functions::createAInstrumentButton(
    std::shared_ptr<MediatorType> mediator, const std::string& fileWithTextureName
)
{
    auto buttonSprite  = std::unique_ptr<ISprite>(ISprite::create());
    auto buttonTexture = std::unique_ptr<ITexture>(ITexture::create());

    buttonTexture.get()->loadFromFile(fileWithTextureName);

    buttonSprite->setTexture(buttonTexture.get());

    std::unique_ptr<ButtonType> button{ new ButtonType(std::move(buttonSprite), 
                                                       std::move(buttonTexture)) };

    button->setMediator(mediator);

    return std::unique_ptr<ASpritedBarButton>(button.release());
}

} // namespace instrument_button_functions

} // namespace ps

#endif // PLUGINS_PLUGIN_LIB_TOOLBAR_BUTTON_HPP