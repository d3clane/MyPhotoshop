#include "api/api_actions.hpp"

#include <deque>
#include <cassert>

namespace psapi
{

class ActionController : public AActionController
{
public:
    bool execute(std::unique_ptr<IAction> action) override;
    bool undo() override;
    bool redo() override;

private:
    static const size_t kMaxActions = 256;

    int currentPos_ = -1;
    std::deque<std::unique_ptr<IUndoableAction>> actions_;
};

bool ActionController::execute(std::unique_ptr<IAction> action)
{
    if (!actionExecute(action.get()))
        return false;

    auto beginEraseIt = actions_.begin() + (currentPos_ + 1);
    if (beginEraseIt != actions_.end())
    {
        actions_.erase(beginEraseIt, actions_.end());
    }

    if (isUndoableAction(action.get()))
    {
        actions_.push_back(std::unique_ptr<IUndoableAction>(
            static_cast<IUndoableAction*>(action.release())
        ));

        currentPos_++;
    }

    return true;
}

bool ActionController::undo()
{
    assert(static_cast<size_t>(currentPos_) < actions_.size());

    if (currentPos_ < 0)
        return false;

    actionUndo(actions_[static_cast<size_t>(currentPos_)].get());
    
    currentPos_--;

    return true;
}

bool ActionController::redo()
{
    if (static_cast<size_t>(currentPos_) == actions_.size() - 1)
        return false;
    
    currentPos_++;

    actionRedo(actions_[static_cast<size_t>(currentPos_)].get());

    return true;
}

AActionController* getActionController()
{
    static ActionController actionController;
    return &actionController;
}

} // namespace psapi
