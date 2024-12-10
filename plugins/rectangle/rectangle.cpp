#include "rectangle.hpp"

#include "pluginLib/shapeButtons/shapeButton.hpp"

bool onLoadPlugin() // onLoadPlugin
{
    return instrument_button_functions::instrumentButtonOnLoadPlugin<
        ShapeButton<IRectangleShape>, MediatorType>("media/textures/paintbrush.png");
}

void onUnloadPlugin()
{
    return;
}
