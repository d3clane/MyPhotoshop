#ifndef PLUGINS_PLUGIN_LIB_WINDOWS_ACTIONS_HPP
#define PLUGINS_PLUGIN_LIB_WINDOWS_ACTIONS_HPP

namespace ps
{

class IAction
{
public:
    virtual void operator()() = 0;

    virtual ~IAction() = default;
};

} // namespace ps

#endif // PLUGINS_PLUGIN_LIB_WINDOWS_ACTIONS_HPP
