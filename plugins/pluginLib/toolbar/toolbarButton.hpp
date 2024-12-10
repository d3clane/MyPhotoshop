#ifndef PLUGINS_PLUGIN_LIB_TOOLBAR_BUTTON_HPP
#define PLUGINS_PLUGIN_LIB_TOOLBAR_BUTTON_HPP

#include "api/api_bar.hpp"
#include "pluginLib/bars/ps_bar.hpp"
#include "pluginLib/instrumentBar/instrumentBar.hpp"

#include <cassert>

namespace ps
{

namespace instrument_button_functions
{

void updateInstrumentBarState(IBar* instrumentBar, IBarButton::State stateNow);

// This two functions have to be called on each update of the instrument button.
std::unique_ptr<IAction> createActionInstrumentBar(IBar* instrumentBar, IBarButton::State stateNow,
                                                   const IRenderWindow* renderWindow, const Event& event);

void drawInstrumentBar(IBar* instrumentBar, IRenderWindow* renderWindow);

template<typename ButtonType>
bool instrumentButtonOnLoadPlugin(const std::string& fileWithTextureName);

} // namespace instrument_button_functions 


// ------------------------------------implementation------------------------------------

namespace instrument_button_functions
{

namespace static_functions
{

template<typename ButtonType>
std::unique_ptr<ButtonType> createAInstrumentButton(const std::string& fileWithTextureName);

} // namespace static_functions

template<typename ButtonType>
bool instrumentButtonOnLoadPlugin(const std::string& fileWithTextureName)
{
    IWindowContainer* rootWindow = getRootWindow();
    assert(rootWindow);
    auto toolbar = static_cast<IBar*>(rootWindow->getWindowById(kToolBarWindowId));
    assert(toolbar);

    auto button = static_functions::createAInstrumentButton<ButtonType>(            
        fileWithTextureName
    );

    toolbar->addWindow(std::move(button));

    return true;
}

template<typename ButtonType>
std::unique_ptr<ButtonType> static_functions::createAInstrumentButton(
    const std::string& fileWithTextureName)
{
    auto buttonSprite  = std::unique_ptr<ISprite>(ISprite::create());
    auto buttonTexture = std::unique_ptr<ITexture>(ITexture::create());

    buttonTexture.get()->loadFromFile(fileWithTextureName);

    buttonSprite->setTexture(buttonTexture.get());

    return std::make_unique<ButtonType>(std::move(buttonSprite), std::move(buttonTexture));
}

} // namespace instrument_button_functions

} // namespace ps

#endif // PLUGINS_PLUGIN_LIB_TOOLBAR_BUTTON_HPP