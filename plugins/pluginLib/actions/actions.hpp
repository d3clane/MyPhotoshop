#ifndef PLUGINS_PLUGIN_LIB_ACTIONS_ACTIONS_HPP
#define PLUGINS_PLUGIN_LIB_ACTIONS_ACTIONS_HPP  

#include "api/api_actions.hpp"

namespace ps
{

using namespace psapi;

template<typename T>
class UpdateCallbackAction : public IAction
{
public:
    UpdateCallbackAction(T& object,
                         const IRenderWindow* renderWindow, 
                         const Event& event) : obj_(object),
                                               renderWindow_(renderWindow), 
                                               event(event) {}

    bool execute(const Key& /*key*/) override { return obj_.update(renderWindow_, event); }
    bool isUndoable(const Key& /*key*/) override { return false; }

private:
    T& obj_;
    const IRenderWindow* renderWindow_;
    const Event event;
};

} // namespace ps

#endif // PLUGINS_PLUGIN_LIB_ACTIONS_ACTIONS_HPP