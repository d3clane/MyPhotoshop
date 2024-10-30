#include "pluginsSrc/canvas/canvas.hpp"
#include "pluginsSrc/toolbar/toolbar.hpp"
#include "pluginsSrc/brush/brush.hpp"
#include "pluginsSrc/spray/spray.hpp"

#include "api/api_sfm.hpp"

int main()
{
    auto renderWindow = psapi::IRenderWindow::create(1920, 1080, "PSAPI");

    loadPlugin2();
    loadPlugin();
    loadPlugin1();
    loadPlugin3();

    auto rootWindow = psapi::getRootWindow();
    
    while (renderWindow->isOpen())
    {
        psapi::sfm::Event event;
        if (renderWindow->pollEvent(event))
        {
            if (event.type == psapi::sfm::Event::Closed)
                renderWindow->close();
        }

        renderWindow->clear();

        rootWindow->draw(renderWindow.get());
        rootWindow->update(renderWindow.get(), event);

        renderWindow->display();
    }
}