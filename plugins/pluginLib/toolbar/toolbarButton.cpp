#include "toolbarButton.hpp"

#include <cassert>

namespace ps
{

void instrument_button_functions::updateInstrumentBarState(IBar* instrumentBar, IBarButton::State stateNow)
{
    assert(instrumentBar);

    if (stateNow != ABarButton::State::Released)
        instrumentBar->forceDeactivate();
    else /* (stateNow == ABarButton::State::Released) */
        instrumentBar->forceActivate();
}

void instrument_button_functions::updateInstrumentBar(IBar* instrumentBar, IBarButton::State stateNow,
                                                      const IRenderWindow* renderWindow, const Event& event)
{
    assert(instrumentBar);

    instrument_button_functions::updateInstrumentBarState(instrumentBar, stateNow);
    instrumentBar->update(renderWindow, event);
}

void instrument_button_functions::drawInstrumentBar(IBar* instrumentBar, IRenderWindow* renderWindow)
{
    instrumentBar->draw(renderWindow);
}

} // namespace ps