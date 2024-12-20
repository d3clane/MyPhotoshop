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
#include <SFML/Graphics.hpp>

using namespace ps;
using namespace psapi;
using namespace psapi::sfm;

namespace
{

// TODO: buttons can't be pressed (like pressed is always flushed) because file names are always dying and
// creating again. On each update scanning dir for files and deleting old ones -> all buttons dies every cycle.

template<typename SubButtonsType>
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

// TODO: I probably have copypaste in many places where I want to create an Action that to do something
// on different states of the button. Could simply create actions for each state and call them. That's all.

class FileSaveButton : public ANamedBarButton
{
public:
    FileSaveButton(wid_t id, std::string filename, std::unique_ptr<IFont> font);

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

template<typename MenuButtonType>
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
            font->loadFromFile("assets/fonts/arial.ttf");

            menu->addWindow(std::make_unique<MenuButtonType>(filesIdsBegin + filesCnt, 
                                                             filename, std::move(font)));

            ++filesCnt;
        }
    }
}

}

template<typename SubButtonsType>
std::unique_ptr<IAction> FilesList<SubButtonsType>::createAction(const IRenderWindow* renderWindow,
                                                                 const Event& event)
{
    updateMenuChildren<SubButtonsType>(this);

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
    bool loadImageRes = image->loadFromFile(filename_);
    assert(loadImageRes);

    ICanvas* canvas = static_cast<ICanvas*>(getRootWindow()->getWindowById(kCanvasWindowId));
    assert(canvas);

    ILayer* activeLayer = canvas->getLayer(canvas->getActiveLayerIndex());

    copyImageToLayer(activeLayer, image.get(), image->getPos());

    state_ = State::Normal;

    return updatedState;
}

// File save button

namespace 
{

sf::Image copyIImageToSfImage(const IImage* image)
{
    sf::Image sfImage;
    sfImage.create(image->getSize().x, image->getSize().y);

    for (unsigned x = 0; x < image->getSize().x; ++x)
    {
        for (unsigned y = 0; y < image->getSize().y; ++y)
        {
            Color color = image->getPixel({x, y});
            sfImage.setPixel(x, y, sf::Color(color.r, color.g, color.b, color.a));
        }
    }

    return sfImage;
}

bool saveToFile(const std::string& filename, const IImage* sfmImage)
{
    sf::Image image = copyIImageToSfImage(sfmImage);

    return image.saveToFile(filename);
}

} // namespace anonymous

FileSaveButton::FileSaveButton(wid_t id, std::string filename, std::unique_ptr<IFont> font)
{
    id_ = id;
    filename_ = filename;
    font_ = std::move(font);

    name_ = IText::create();
    name_->setFont(font_.get());
    name_->setString(filename);
}

std::unique_ptr<IAction> FileSaveButton::createAction(const IRenderWindow* renderWindow,
                                                      const Event& event)
{
    return std::make_unique<UpdateCallbackAction<FileSaveButton>>(*this, renderWindow, event);
}

bool FileSaveButton::update(const IRenderWindow* renderWindow, const Event& event)
{
    bool updatedState = updateState(renderWindow, event);

    if (state_ != State::Released)
        return false;

    ICanvas* canvas = static_cast<ICanvas*>(getRootWindow()->getWindowById(kCanvasWindowId));
    assert(canvas);

    ILayer* activeLayer = canvas->getLayer(canvas->getActiveLayerIndex());
    assert(activeLayer);

    std::unique_ptr<IImage> image = copyLayerToImage(activeLayer, activeLayer->getSize());
    bool saveImageRes = saveToFile(filename_, image.get());
    assert(saveImageRes);

    state_ = State::Normal;

    return updatedState;
}

} // namespace anonymous

namespace 
{

template<typename SubButtonsType>
void loadPlugin(const char* pluginName)
{
    std::unique_ptr<IFont> font = IFont::create();
    font->loadFromFile("assets/fonts/arial.ttf");
    std::unique_ptr<IText> name = IText::create();
    name->setFont(font.get());
    name->setString(pluginName);


    IMenuButton* fileMenu = static_cast<IMenuButton*>(getRootWindow()->getWindowById(kMenuFileId));

    fileMenu->addMenuItem(std::make_unique<MenuButton>(
        kInvalidWindowId, std::move(name), std::move(font), 
        std::make_unique<FilesList<SubButtonsType>>(), MenuButton::SubMenuSpawningDirection::Right)); 
}

} // namespace anonymous

bool onLoadPlugin()
{
    loadPlugin<FileOpenButton>("Open");
    loadPlugin<FileSaveButton>("Save");

    return true;
}

void onUnloadPlugin()
{
    return;
}