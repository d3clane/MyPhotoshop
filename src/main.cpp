#include "plugins/canvas/canvas.hpp"
#include "plugins/toolbar/toolbar.hpp"
#include "plugins/brush/brush.hpp"
#include "plugins/spray/spray.hpp"

#include "api/api_sfm.hpp"

#include <SFML/Graphics.hpp>

#include <dlfcn.h>

void loadPlugin(const char* libName)
{
    void *handle = dlopen(libName, RTLD_NOW);

    if (!handle)
    {
        std::cerr << "Ошибка загрузки библиотеки: " << dlerror() << std::endl;
        return;
    }

    void (*load_func)() = (void (*)())dlsym(handle, "loadPlugin");

    if (!load_func) 
    {
        std::cerr << "Ошибка получения функции load(): " << dlerror() << std::endl;
        return;
    }

    load_func();

    //dlclose(handle);
}

int main()
{
    auto renderWindow = psapi::IRenderWindow::create(1920, 1080, "photoshop");
    //renderWindow->setFps(60);
    
    loadPlugin("libs/lib_canvas.dylib");
    loadPlugin("libs/lib_toolbar.dylib");
    loadPlugin("libs/lib_spray.dylib");
    loadPlugin("libs/lib_brush.dylib");
    loadPlugin("libs/lib_line.dylib");
    loadPlugin("libs/lib_ellipse.dylib");

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

#if 0
int main()
{
    sf::Window window(sf::VideoMode(800, 600), "Touch Input Example");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::MouseWheelScrolled:
                    if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) 
                    {
                        std::cerr << "VERTICAL SCROLL: " << event.mouseWheelScroll.delta << std::endl;
                    }
                    else
                    {
                        std::cerr << "HORIZONTAL SCROLL: " << event.mouseWheelScroll.delta << std::endl;
                    }
                    // You can use event.touch.x and event.touch.y to track the movement
                    break;

                case sf::Event::Closed:
                    window.close();
                    break;

                default:
                    break;
            }
        }
    }

}
#endif