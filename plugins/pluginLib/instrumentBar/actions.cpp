#include "actions.hpp"

namespace ps
{

AChangeColorAction::AChangeColorAction(const Color& color) : color_(color) 
{
}

Color AChangeColorAction::getColor() const
{ 
    return color_; 
}

ChangeFillColorAction::ChangeFillColorAction(const Color& color, 
                                             std::shared_ptr<AFillPropertiesMediator> mediator)
    : AChangeColorAction(color), mediator_(mediator)
{
}

void ChangeFillColorAction::operator()()
{
    mediator_->fillProperties().color = color_;
}

ChangeOutlineColorAction::ChangeOutlineColorAction(const Color& color, 
                                             std::shared_ptr<AOutlinePropertiesMediator> mediator)
    : AChangeColorAction(color), mediator_(mediator)
{
}

void ChangeOutlineColorAction::operator()()
{
    mediator_->outlineProperties().color = color_;
}

} // namespace ps