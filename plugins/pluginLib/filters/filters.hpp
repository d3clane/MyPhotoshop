#ifndef PLUGINS_PLUGIN_LIB_FILTERS_FILTERS_HPP
#define PLUGINS_PLUGIN_LIB_FILTERS_FILTERS_HPP

#include "api/api_sfm.hpp"
#include "api/api_system.hpp"

#include "pluginLib/bars/ps_bar.hpp"

#include <vector>

namespace ps
{

using namespace psapi;
using namespace psapi::sfm;

class FilterAction : public ANamedBarButton
{
public:
    
};

class FilterButton : public ANamedBarButton
{
public:
    FilterButton(std::unique_ptr<IText> name, std::unique_ptr<IFont> font,
                 std::unique_ptr<IAction> filterAction);

    std::unique_ptr<IAction> createAction(const IRenderWindow* renderWindow, 
                                          const Event& event) override;
    
    bool update(const IRenderWindow* renderWindow, const Event& event);

private:
    std::unique_ptr<IAction> filterAction_;
};

std::vector<std::vector<Color>> getNegative (const std::vector<std::vector<Color>>& pixels);
std::vector<std::vector<Color>> getBasRelief(const std::vector<std::vector<Color>>& pixels, 
                                             const std::vector<std::vector<Color>>& negative);

std::vector<std::vector<Color>> getBoxBlured(const std::vector<std::vector<Color>>& pixels);

std::vector<std::vector<Color>> getUnsharpMasked(const std::vector<std::vector<Color>>& pixels);

} // namespace ps

#endif // PLUGINS_PLUGIN_LIB_FILTERS_FILTERS_HPP