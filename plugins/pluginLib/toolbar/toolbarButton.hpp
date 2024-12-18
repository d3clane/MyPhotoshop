#ifndef PLUGINS_PLUGIN_LIB_TOOLBAR_BUTTON_HPP
#define PLUGINS_PLUGIN_LIB_TOOLBAR_BUTTON_HPP

#include "api/api_bar.hpp"

#include "pluginLib/bars/ps_bar.hpp"
#include "pluginLib/instrumentBar/instrumentBar.hpp"
#include "pluginLib/canvas/canvas.hpp"

#include <cassert>

namespace ps
{

class CanvasSaverProcedure
{
public:
    CanvasSaverProcedure();

    void canvasSaveBegin();
    void canvasSaveEnd();
    bool isSavingComplete() const;
    std::unique_ptr<CanvasSaverAction> flushCanvasSaving();

private:
    bool savingIsComplete_ = false;
    std::unique_ptr<CanvasSaverAction> canvasSaving_;
};

class AInstrumentButton : public ASpritedBarButton
{
protected:
    void updateOptionsBar(State stateNow, State prevState);

protected:
    IColorPalette* colorPalette_ = nullptr;
    IThicknessOption* thicknessOption_ = nullptr;
    
    CanvasSaverProcedure canvasSaver_;
};

namespace instrument_button_functions
{

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

    auto button = std::make_unique<ButtonType>(std::move(buttonSprite), std::move(buttonTexture));

    return button;
}

} // namespace instrument_button_functions

} // namespace ps

#endif // PLUGINS_PLUGIN_LIB_TOOLBAR_BUTTON_HPP