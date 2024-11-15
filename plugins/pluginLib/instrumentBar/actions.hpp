#ifndef PLUGINS_PLUGIN_LIB_INSTRUMENTS_BAR_ACTIONS_HPP
#define PLUGINS_PLUGIN_LIB_INSTRUMENTS_BAR_ACTIONS_HPP

#include "mediator.hpp"
#include <memory>

namespace ps
{

class ColorButton;

class IAction
{
public:
    virtual void operator()() = 0;

    virtual ~IAction() = default;
};

class AChangeColorAction : public IAction
{
public:
    AChangeColorAction(const Color& color);

    Color getColor() const;

protected: 
    Color color_;
};

class ChangeFillColorAction : public AChangeColorAction 
{
public:
    ChangeFillColorAction(const Color& color, std::shared_ptr<AFillPropertiesMediator> mediator);
    
    void operator()() override;

private:
    std::shared_ptr<AFillPropertiesMediator> mediator_;
};

class ChangeOutlineColorAction : public AChangeColorAction
{
public:
    ChangeOutlineColorAction(const Color& color, std::shared_ptr<AOutlinePropertiesMediator> mediator);

    void operator()() override;

private:
    std::shared_ptr<AOutlinePropertiesMediator> mediator_;
};

} // namespace ps


#endif // PLUGINS_PLUGIN_LIB_INSTRUMENTS_BAR_ACTIONS_HPP