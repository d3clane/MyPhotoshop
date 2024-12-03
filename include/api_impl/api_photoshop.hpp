#ifndef API_IMPL_API_PHOTOSHOP_HPP
#define API_IMPL_API_PHOTOSHOP_HPP

#include "api/api_photoshop.hpp"
#include <vector>


namespace psapi
{

class RootWindow : public IRootWindow
{
public:
    void draw(IRenderWindow* renderWindow) override;
    std::unique_ptr<IAction> createAction(const IRenderWindow* renderWindow,
                                          const Event& event) override;
    wid_t getId() const override;

    void setParent(const IWindow* parent) override;
    void forceActivate()                  override;
    void forceDeactivate()                override;

    bool isActive() const override;

    void addWindow(std::unique_ptr<IWindow> window) override; 
    void removeWindow(wid_t id) override;
    IWindow* getWindowById(wid_t id) override;
    const IWindow* getWindowById(wid_t id) const override;

    vec2i getPos()  const           override;
    vec2u getSize() const           override;
    void setSize(const vec2u& size) override;
    void setPos (const vec2i& pos)  override;

    layer_id_t getUpperLayerId() const override;
    layer_id_t increaseLayerId()       override;
    layer_id_t decreaseLayerId()       override;

    IRenderWindow* getRenderWindow();

    static RootWindow* create(vec2u size);

private:
    RootWindow() = default;
    std::vector<std::unique_ptr<IWindow>> windows_;

    std::unique_ptr<IRenderWindow> renderWindow_;
    layer_id_t maxLayerId_;
};

} // namespace psapi

#endif // API_IMPL_API_PHOTOSHOP_HPP