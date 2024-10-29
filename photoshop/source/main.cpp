#include "api_sfm.hpp"
#include "sfm_implementation.h"
#include "ps_plugin_toolbar.h"
#include "spray_plugin.h"
#include "photoshopAPI/api_canvas.hpp"
#include "api_implementation/ps_canvas.h"

#include <dlfcn.h>

using namespace psapi;

int main(int argc, char *argv[]) {

    sfm::RenderWindow window(800, 600, "StandPhotoshop");

    loadPlugin();
    assert(getRootWindow()->getWindowById(kToolBarWindowId));
    loadPlugin1();

    std::cout << "DD\n";

    std::unique_ptr<IWindow> canvas{new ps::Canvas({300, 200})};
    
    getRootWindow()->addWindow(std::move(canvas));

    while (window.isOpen()) {
        sfm::Event event;
        if (window.pollEvent(event)) {

            if (event.type == sfm::Event::Closed) {
                window.close();
            }
        }
        window.display();
        window.clear();

        getRootWindow()->draw(&window);
            getRootWindow()->update(&window, event);
    }

    return 0;
}
