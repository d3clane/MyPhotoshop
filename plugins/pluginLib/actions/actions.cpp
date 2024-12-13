#include "actions.hpp"

using namespace psapi;

namespace ps
{

#if 0
ButtonContinuousAction::ButtonContinuousAction(IBarButton* button, 
    const IRenderWindow* renderWindow, const Event& event,
    std::unique_ptr<IAction> releaseAction, std::unique_ptr<IAction> otherAction) 
    : button_(button), renderWindow_(renderWindow), event_(event), 
        releaseAction_(std::move(releaseAction)), otherAction_(std::move(otherAction)),
        previousState_(button->getState()) 
{
}

bool ButtonContinuousAction::isUndoable(const Key& key)
{
    return releaseAction_->isUndoable(key);
}

bool ButtonContinuousAction::execute(const Key& key)
{
    previousState_ = button_->getState();

    bool updatedState = updateState(renderWindow, event);

    updateOptionsBar(state_, prevState);

    if (state_ != State::Released)
        return updatedState;
}

#endif



}
