#include "pluginsSrc/canvas/canvas.hpp"
#include "pluginsSrc/toolbar/toolbar.hpp"
#include "pluginsSrc/brush/brush.hpp"
#include "pluginsSrc/spray/spray.hpp"

#include "api/api_sfm.hpp"

#include <dlfcn.h>

void callLoader(const char* libName)
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

    dlclose(handle);
}


int main()
{
    std::cerr << "MAIN\n";
    auto renderWindow = psapi::IRenderWindow::create(1920, 1080, "PSAPI");

#if 0
    loadPlugin2();
    std::cerr << "LOAD PLUGIN\n";
    loadPlugin ();
    std::cerr << "LOAD PLUGIN 1\n";
    loadPlugin1();
    std::cerr << "LOAD PLUGIN 3\n";
    //loadPlugin3();
#endif

    callLoader("lib_canvas.dylib");
    callLoader("lib_toolbar.dylib");
    callLoader("lib_spray.dylib");
    callLoader("lib_brush.dylib");
    
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