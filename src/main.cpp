#include "pluginsSrc/canvas/canvas.hpp"
#include "pluginsSrc/toolbar/toolbar.hpp"
#include "pluginsSrc/brush/brush.hpp"
#include "pluginsSrc/spray/spray.hpp"

#include "api/api_sfm.hpp"

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
    auto renderWindow = psapi::IRenderWindow::create(1920, 1080, "PSAPI");
    renderWindow->setFps(60);
    
    loadPlugin("libs/lib_canvas.dylib");
    loadPlugin("libs/lib_toolbar.dylib");
    loadPlugin("libs/lib_spray.dylib");
    loadPlugin("libs/lib_brush.dylib");
    loadPlugin("libs/lib_line.dylib");
    
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