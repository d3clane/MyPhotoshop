#ifndef PLUGINS_PLUGIN_LIB_MEDIATOR_HPP
#define PLUGINS_PLUGIN_LIB_MEDIATOR_HPP

#include "api/api_sfm.hpp"

namespace ps
{

using namespace psapi;
using namespace psapi::sfm;
    
struct DrawingProperties
{
    Color color;
    unsigned thickness;
};

class AFillPropertiesMediator
{
public:
    DrawingProperties& getFillProperties()             { return fillProperties_; }
    const DrawingProperties& getFillProperties() const { return fillProperties_; }

    virtual ~AFillPropertiesMediator() = default;
private:
    DrawingProperties fillProperties_;
};

class AOutlinePropertiesMediator
{
public:
    DrawingProperties& getOutlineProperties()             { return outlineProperties_; }
    const DrawingProperties& getOutlineProperties() const { return outlineProperties_; }

private:
    DrawingProperties outlineProperties_;
};

class APropertiesMediator : public AFillPropertiesMediator, public AOutlinePropertiesMediator
{
};

} // namespace ps

#endif // PLUGINS_PLUGIN_LIB_MEDIATOR_HPP