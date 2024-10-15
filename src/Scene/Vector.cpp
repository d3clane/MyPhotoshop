#include "Scene/Vector.hpp"

namespace Scene
{

DrawableVector::DrawableVector(const Engine::Vector& v, const Engine::Point& beginPos) : 
    vector_(v), beginPos(beginPos) 
{
}

void DrawableVector::draw(Graphics::RenderWindow& renderWindow, const Engine::CoordsSystem& cs)
{
    Engine::Point endPos = beginPos + vector_;

    Graphics::WindowPoint beginPosInWindow = cs.getPointInWindow(beginPos);
    Graphics::WindowPoint endPosInWindow   = cs.getPointInWindow(endPos);

    Graphics::WindowLine mainLine{beginPosInWindow, endPosInWindow};

    renderWindow.drawLine(mainLine);

    Engine::Vector normal = vector_.getPerpendicular();

    const double prettyLength = 0.05 * vector_.length();
    normal.length(prettyLength);

    Engine::Vector coDirectionalVector = vector_;
    coDirectionalVector.length(prettyLength);

    Engine::Vector arrowVector1 =  normal - coDirectionalVector;
    Engine::Vector arrowVector2 = -normal - coDirectionalVector;

    Engine::Point endArrowVector1 = endPos + arrowVector1;
    Engine::Point endArrowVector2 = endPos + arrowVector2;

    Graphics::WindowPoint endArrowVector1InWindow = cs.getPointInWindow(endArrowVector1);
    Graphics::WindowPoint endArrowVector2InWindow = cs.getPointInWindow(endArrowVector2);

    renderWindow.drawLine({endPosInWindow, endArrowVector1InWindow});
    renderWindow.drawLine({endPosInWindow, endArrowVector2InWindow});
}

} // namespace Scene
