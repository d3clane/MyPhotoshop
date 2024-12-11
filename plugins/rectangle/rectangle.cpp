#include "rectangle.hpp"

#include "pluginLib/shapeButtons/shapeButton.hpp"

bool onLoadPlugin() // onLoadPlugin
{
    return instrument_button_functions::instrumentButtonOnLoadPlugin<
        ShapeButton<IRectangleShape>>("media/textures/rectangle.png");
}

void onUnloadPlugin()
{
    return;
}
