#include "ellipse.hpp"

#include "pluginLib/shapeButtons/shapeButton.hpp"

bool onLoadPlugin()
{
    return instrument_button_functions::instrumentButtonOnLoadPlugin<
        ShapeButton<IEllipseShape>>("media/textures/ellipse.png");
}

void onUnloadPlugin()
{
    return;
}
