#include "pluginsSrc/canvas/canvas.hpp"
#include "api/api_sfm.hpp"

int main()
{
    auto renderWindow = psapi::IRenderWindow::create(800, 600, "PSAPI");

    loadPlugin2();

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