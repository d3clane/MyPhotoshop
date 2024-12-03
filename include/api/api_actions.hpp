#ifndef API_ACTIONS_HPP
#define API_ACTIONS_HPP

#include <memory>

namespace psapi
{


class AActionController;


class IAction
{
public:
    virtual ~IAction() = default;

    class Key
    {
        friend class AActionController;
        Key() {}
    };

    virtual bool execute   (const Key& key) = 0;
    virtual bool isUndoable(const Key& key) = 0;
};

class IUndoableAction : public IAction
{
public:
    virtual bool undo(const Key& key) = 0;
    virtual bool redo(const Key& key) = 0;
};

class AActionController
{
public:
    virtual ~AActionController() = default;

    virtual bool execute(std::unique_ptr<IAction> action) = 0;

    virtual bool undo() = 0;
    virtual bool redo() = 0;

protected:
    bool actionExecute(IAction* action)
    {
        return action->execute(IAction::Key());
    }

    bool actionUndo(IUndoableAction* action)
    {
        return action->undo(IAction::Key());
    }

    bool actionRedo(IUndoableAction* action)
    {
        return action->redo(IAction::Key());
    }
    bool isUndoableAction(IAction* action)
    {
        return action->isUndoable(IAction::Key());
    }
};

AActionController* getActionController();


} // namespace psapi


#endif // API_ACTIONS_HPP
