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

// Canvas saver procedure implementation

CanvasSaverProcedure::CanvasSaverProcedure()
{
    canvasSaving_ = std::make_unique<CanvasSaverAction>();
}

bool CanvasSaverProcedure::isSavingComplete() const
{
    return savingIsComplete_;
}

std::unique_ptr<CanvasSaverAction> CanvasSaverProcedure::flushCanvasSaving()
{
    std::unique_ptr<CanvasSaverAction> savingAction = std::move(canvasSaving_); 
    canvasSaving_ = std::make_unique<CanvasSaverAction>();
    savingIsComplete_ = false;

    return savingAction;
}

void CanvasSaverProcedure::canvasSaveBegin()
{
    ICanvas* canvas = static_cast<ICanvas*>(getRootWindow()->getWindowById(kCanvasWindowId));
    assert(canvas);

    canvasSaving_->setPastSnapshot(canvas->save());
}

void CanvasSaverProcedure::canvasSaveEnd()
{
    ICanvas* canvas = static_cast<ICanvas*>(getRootWindow()->getWindowById(kCanvasWindowId));
    assert(canvas);

    canvasSaving_->setFutureSnapshot(canvas->save());
    savingIsComplete_ = true;
}

} // namespace ps