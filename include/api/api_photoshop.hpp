/**
 * @file api_photoshop.hpp
 * @brief API for Photoshop
 */

#ifndef API_PHOTOSHOP_HPP
#define API_PHOTOSHOP_HPP

#include "api_actions.hpp"
#include "api_sfm.hpp"

#include <cstdint>
#include <memory>

namespace psapi
{


using sfm::IRenderWindow;
using sfm::Event;

/**
 * @brief Alias for window identifier type.
 */
using wid_t = int64_t;


const wid_t kRootWindowId = 0;


using sfm::vec2i;
using sfm::vec2u;
using sfm::vec2f;
using sfm::vec2d;

/** @brief Invalid window ID constant. */
const wid_t kInvalidWindowId = -1;


/**
 * @brief Get size and position of objects in Photoshop
 */
sfm::IntRect getCanvasIntRect();
sfm::IntRect getToolbarIntRect();
sfm::IntRect getOptionsBarIntRect();
sfm::IntRect getMenuBarIntRect();

/**
 * @brief Get the size of the screen
 *
 * @return psapi::sfm::vec2i
 */
psapi::sfm::vec2u getScreenSize();

/**
 * @brief Interface representing a basic window.
 */
class IWindow
{
public:
    virtual ~IWindow() = default;

    /**
     * @brief Renders the window.
     * @param renderWindow The render target.
     */
    virtual void draw(IRenderWindow* renderWindow) = 0;

    /**
     * @brief creates an action for action controller to execute
     *
     */
    virtual std::unique_ptr<IAction> createAction(const IRenderWindow* renderWindow, const Event& event) = 0;

    /**
     * @brief Gets the unique ID of the window.
     * @return The window ID, or kInvalidWindowId if not set.
     */
    virtual wid_t getId() const = 0;

    /**
     * @brief Retrieves a window by its ID from this window or its children.
     * @param id The ID of the window to search for.
     * @return A pointer to the window if found, or nullptr if not found.
     */
    virtual IWindow* getWindowById(wid_t id) = 0;

    /**
     * @brief Retrieves a window by its ID from this window or its children (const version).
     * @param id The ID of the window to search for.
     * @return A const pointer to the window if found, or nullptr if not found.
     */
    virtual const IWindow* getWindowById(wid_t id) const = 0;

    /**
     * @brief Gets the position of the window.
     * @return The absolute window position as a vec2i.
     */
    virtual vec2i getPos() const = 0;

    /**
     * @brief Gets the size of the window.
     * @return The window size as a vec2u.
     */
    virtual vec2u getSize() const = 0;

    /**
     * @brief Sets the size of the window.
     * @param pos The new window position as a vec2i.
     */
    virtual void setSize(const vec2u& size) = 0;

    /**
     * @brief Sets the position of the window.
     * @param pos The new window position as a vec2i.
     */
    virtual void setPos(const vec2i& pos) = 0;

    /**
     * @brief Sets the parent of this window.
     * @param parent A pointer to the parent window.
     */
    virtual void setParent(const IWindow* parent) = 0;

    /**
     * @brief Forces the window to activate.
     */
    virtual void forceActivate() = 0;

    /**
     * @brief Forces the window to deactivate.
     */
    virtual void forceDeactivate() = 0;

    /**
     * @brief Checks whether the window is active.
     */
    virtual bool isActive() const = 0;

    /**
     * @brief Checks if the window is a window container.
     * @return True if the window is a window container, false otherwise.
     */
    virtual bool isWindowContainer() const = 0;
};

/**
 * @brief Interface representing a container of windows.
 */
class IWindowContainer : public IWindow
{
public:
    /**
     * @brief Adds a new window to the container.
     * @param window The window to add.
     */
    virtual void addWindow(std::unique_ptr<IWindow> window) = 0;

    /**
     * @brief Removes a window from the container by its ID.
     * @param id The ID of the window to remove.
     */
    virtual void removeWindow(wid_t id) = 0;

    virtual bool isWindowContainer() const override;
};


/**
 * @brief Function pointer type for general-purpose functions.
 */
using generalFunction = void* (*)(void*);

/**
 * @brief Retrieves a general-purpose function by name.
 * @param name The name of the function.
 * @return A pointer to the function, or nullptr if not found.
 */
generalFunction getGeneralFunction(const std::string& name);


using layer_id_t = int64_t;

class IRootWindow : public IWindowContainer
{
public:
    virtual ~IRootWindow() = default;

    virtual layer_id_t getUpperLayerId() const = 0;
    virtual layer_id_t increaseLayerId() = 0;
    virtual layer_id_t decreaseLayerId() = 0;
};


/**
 * @brief Retrieves the root window of the application.
 * @return A pointer to the root window container.
 */
IRootWindow* getRootWindow();



} // namespace psapi

#endif // API_PHOTOSHOP_HPP
