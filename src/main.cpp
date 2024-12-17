#include "api/api_sfm.hpp"
#include "api/api_photoshop.hpp"
#include "api_impl/api_photoshop.hpp"

#include <iostream>
#include <cassert>

#include <SFML/Graphics.hpp>

#include <dlfcn.h>

using namespace psapi;
using namespace psapi::sfm;

void loadPlugin(const char* libName)
{
    void *handle = dlopen(libName, RTLD_NOW);

    if (!handle)
    {
        std::cerr << "Ошибка загрузки библиотеки: " << dlerror() << std::endl;
        return;
    }

    void (*load_func)() = (void (*)())dlsym(handle, "onLoadPlugin");

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
    RootWindow* rootWindow = RootWindow::create(vec2u{1920, 1080});
    AActionController* actionController = getActionController();
    
    loadPlugin("libs/lib_canvas.dylib");
    loadPlugin("libs/lib_toolbar.dylib");
    loadPlugin("libs/lib_optionsBar.dylib");
    loadPlugin("libs/lib_menuBar.dylib");
    loadPlugin("libs/lib_brush.dylib");
    loadPlugin("libs/lib_eraser.dylib");
    loadPlugin("libs/lib_line.dylib");
    loadPlugin("libs/lib_ellipse.dylib");
    loadPlugin("libs/lib_rectangle.dylib");
    loadPlugin("libs/lib_negative_filter.dylib");
    loadPlugin("libs/lib_blur_filter.dylib");
    loadPlugin("libs/lib_file_loader.dylib");
    loadPlugin("libs/lib_edit_settings.dylib");
    loadPlugin("libs/lib_bas_relief.dylib");
    loadPlugin("libs/lib_unsharp_mask.dylib");

    IRenderWindow* renderWindow = rootWindow->getRenderWindow();
    assert(renderWindow);
    while (renderWindow->isOpen())
    {
        psapi::sfm::Event event;
        while (renderWindow->pollEvent(event))
        {
            if (event.type == psapi::sfm::Event::Closed)
                renderWindow->close();

            actionController->execute(rootWindow->createAction(renderWindow, event));
        }

        actionController->execute(rootWindow->createAction(renderWindow, event));
        renderWindow->clear();
        rootWindow->draw(renderWindow);
        renderWindow->display();
    }
}
