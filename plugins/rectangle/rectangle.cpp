#include "rectangle.hpp"

#include "pluginLib/shapeButtons/shapeButton.hpp"

bool onLoadPlugin() // onLoadPlugin
{
    return instrument_button_functions::instrumentButtonOnLoadPlugin<
        ShapeButton<IRectangleShape>>("assets/textures/rectangle.png");
}

void onUnloadPlugin()
{
    return;
}
