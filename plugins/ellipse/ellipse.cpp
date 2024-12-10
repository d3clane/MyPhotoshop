#include "ellipse.hpp"

#include "pluginLib/shapeButtons/shapeButton.hpp"

bool onLoadPlugin()
{
    return instrument_button_functions::instrumentButtonOnLoadPlugin<
        ShapeButton<IEllipseShape>, MediatorType>("media/textures/paintbrush.png");
}

void onUnloadPlugin()
{
    return;
}
