#ifndef PLUGINS_PLUGIN_LIB_WINDOWS_CONCRETE_BUTTONS_HPP
#define PLUGINS_PLUGIN_LIB_WINDOWS_CONCRETE_BUTTONS_HPP

#include "api/api_photoshop.hpp"
#include "api/api_bar.hpp"

#include "pluginLib/bars/ps_bar.hpp"
#include "actions.hpp"

#include <string>

namespace ps
{

using namespace psapi;

class ActivatingAction : public IAction
{
public:
    ActivatingAction(IWindow* window, IBarButton* button);
    
    void operator()() override;

private:
    IBarButton* button_;

    IWindow* window_;
};

class ActivatingButton : public ABarButton
{
    // TODO: implement
};

};

#endif // PLUGINS_PLUGIN_LIB_WINDOWS_CONCRETE_BUTTONS_HPP