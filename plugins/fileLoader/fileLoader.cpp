#include "fileLoader.hpp"

#include "api/api_sfm.hpp"
#include "api/api_canvas.hpp"

#include "pluginLib/bars/ps_bar.hpp"
#include "pluginLib/bars/menu.hpp"
#include "pluginLib/actions/actions.hpp"
#include "pluginLib/canvas/canvas.hpp"

#include <dirent.h>
#include <string>
#include <cassert>

using namespace ps;
using namespace psapi;
using namespace psapi::sfm;

namespace
{

class FilesList : public SubMenuBar
{
public:
    std::unique_ptr<IAction> createAction(const IRenderWindow* renderWindow,
                                          const Event& event) override;
};

class FileOpenButton : public ANamedBarButton
{
public:
    FileOpenButton(wid_t id, std::string filename, std::unique_ptr<IFont> font);

    std::unique_ptr<IAction> createAction(const IRenderWindow* renderWindow,
                                          const Event& event) override;

    bool update(const IRenderWindow* renderWindow, const Event& event);

private:
    std::string filename_;
};

namespace 
{

bool isImage(const std::string& filename)
{
    size_t len = filename.size();

    return (len >= 5 && filename.substr(len - 5, 5) == ".jpeg") ||
           (len >= 4 && filename.substr(len - 4, 4) == ".png") || 
           (len >= 4 && filename.substr(len - 4, 4) == ".jpg");
}

void updateMenuChildren(SubMenuBar* menu)
{
    static const wid_t filesIdsBegin = 974012;
    static wid_t filesCnt = 0;

    for (wid_t id = filesIdsBegin; id < filesIdsBegin + filesCnt; ++id)
        menu->removeWindow(id);

    filesCnt = 0;
    
    DIR* dir = opendir(".");
    assert(dir);

    struct dirent* entry;

    while ((entry = readdir(dir)) != nullptr) {
        std::string filename(entry->d_name);
        
        if (entry->d_type != DT_REG || filename == "." || filename == "..")
            continue;
        
        if (isImage(filename))
        {
            std::unique_ptr<IFont> font = IFont::create();
            font->loadFromFile("media/fonts/arial.ttf");

            menu->addWindow(std::make_unique<FileOpenButton>(filesIdsBegin + filesCnt, 
                                                             filename, std::move(font)));

            ++filesCnt;
        }
    }
}

}

std::unique_ptr<IAction> FilesList::createAction(const IRenderWindow* renderWindow,
                                                 const Event& event)
{
    updateMenuChildren(this);

    return SubMenuBar::createAction(renderWindow, event);
}

FileOpenButton::FileOpenButton(wid_t id, std::string filename, std::unique_ptr<IFont> font)
{
    id_ = id;
    filename_ = filename;
    font_ = std::move(font);

    name_ = IText::create();
    name_->setFont(font_.get());
    name_->setString(filename);
}

std::unique_ptr<IAction> FileOpenButton::createAction(const IRenderWindow* renderWindow,
                                                      const Event& event)
{
    return std::make_unique<UpdateCallbackAction<FileOpenButton>>(*this, renderWindow, event);
}

bool FileOpenButton::update(const IRenderWindow* renderWindow, const Event& event)
{
    bool updatedState = updateState(renderWindow, event);

    if (state_ != State::Released)
        return false;

    std::unique_ptr<IImage> image = IImage::create();
    image->loadFromFile(filename_);

    ICanvas* canvas = static_cast<ICanvas*>(getRootWindow()->getWindowById(kCanvasWindowId));
    assert(canvas);

    ILayer* activeLayer = canvas->getLayer(canvas->getActiveLayerIndex());

    copyImageToLayer(activeLayer, image.get(), image->getPos());

    state_ = State::Normal;

    return updatedState;
}

} // namespace anonymous

bool onLoadPlugin()
{
    std::unique_ptr<IFont> font = IFont::create();
    font->loadFromFile("media/fonts/arial.ttf");
    std::unique_ptr<IText> name = IText::create();
    name->setFont(font.get());
    name->setString("Open");

    IMenuButton* fileMenu = static_cast<IMenuButton*>(getRootWindow()->getWindowById(kMenuFileId));

    fileMenu->addMenuItem(std::make_unique<MenuButton>(
        kInvalidWindowId, std::move(name), std::move(font), 
        std::make_unique<FilesList>(), MenuButton::SubMenuSpawningDirection::Right));    

    return true;
}

void onUnloadPlugin()
{
    return;
}