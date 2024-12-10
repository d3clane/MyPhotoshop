#include "toolbarButton.hpp"

#include <cassert>

namespace ps
{

namespace 
{

IColorPalette* createInstrumentColorPalette()
{    
    IOptionsBar* optionsBar = static_cast<IOptionsBar*>(
        getRootWindow()->getWindowById(kOptionsBarWindowId));
    assert(optionsBar);

    optionsBar->removeAllOptions();
    std::unique_ptr<IColorPalette> colorPalette = IColorPalette::create();
    
    IColorPalette* palette = colorPalette.get();

    optionsBar->addWindow(std::move(colorPalette));

    return palette;
}

void clearInstrumentOptions()
{
    IOptionsBar* optionsBar = static_cast<IOptionsBar*>(
        getRootWindow()->getWindowById(kOptionsBarWindowId));
    assert(optionsBar);

    optionsBar->removeAllOptions();
}

} // namespace anonymous

void AInstrumentButton::updateOptionsBar(State stateNow, State prevState)
{
    if (prevState == State::Released && stateNow != State::Released)
    {
        clearInstrumentOptions();
        colorPalette_ = nullptr;
    }

    if (stateNow == State::Released && prevState != State::Released)
    {
        clearInstrumentOptions();
        colorPalette_ = createInstrumentColorPalette();
    } 
}

} // namespace ps