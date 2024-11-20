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
    DrawingProperties& fillProperties()             { return fillProperties_; }
    const DrawingProperties& fillProperties() const { return fillProperties_; }

    virtual ~AFillPropertiesMediator() = default;

private:
    DrawingProperties fillProperties_ = { Color{255, 0, 0, 255}, 4 }; // TODO: temp solution
};

class AOutlinePropertiesMediator
{
public:
    DrawingProperties& outlineProperties()             { return outlineProperties_; }
    const DrawingProperties& outlineProperties() const { return outlineProperties_; }

private:
    DrawingProperties outlineProperties_;
};

class APropertiesMediator : public AFillPropertiesMediator, public AOutlinePropertiesMediator
{
};

} // namespace ps

#endif // PLUGINS_PLUGIN_LIB_MEDIATOR_HPP