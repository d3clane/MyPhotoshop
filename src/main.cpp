#include "pluginsSrc/canvas/canvas.hpp"
#include "pluginsSrc/toolbar/toolbar.hpp"
#include "pluginsSrc/brush/brush.hpp"
#include "pluginsSrc/spray/spray.hpp"

#include "api/api_sfm.hpp"

#include <dlfcn.h>

int main()
{
    auto renderWindow = psapi::IRenderWindow::create(1920, 1080, "PSAPI");

    loadPlugin2();
    loadPlugin ();
    loadPlugin1();
    loadPlugin3();

#if 0
    void *handle = dlopen("libbrush.dylib", RTLD_NOW);

    if (!handle)
    {
        std::cerr << "Ошибка загрузки библиотеки: " << dlerror() << std::endl;
        return 1;
    }

    void (*load_func)() = (void (*)())dlsym(handle, "loadPlugin1");

    if (!load_func) 
    {
        std::cerr << "Ошибка получения функции load(): " << dlerror() << std::endl;
        return 1;
    }

    load_func();
#endif

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

    //dlclose(handle);
}