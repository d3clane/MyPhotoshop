#include <cassert>
#include <cmath>

#include "Utils/Doubles.hpp"

namespace Utils
{

ComparisonResult compare(double a, double b, double eps)
{
    assert(std::isfinite(a));
    assert(std::isfinite(b));
    assert(std::isfinite(eps));

    if (std::abs(a - b) < eps) return ComparisonResult::Equal;
    
    if (a < b)                 return ComparisonResult::Less;

                               return ComparisonResult::Greater;
}

} // namespace Utils