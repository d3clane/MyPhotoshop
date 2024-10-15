#ifndef UTILS_DOUBLES_HPP
#define UTILS_DOUBLES_HPP

namespace Utils
{

static const double EPS = 0.00001;

enum class ComparisonResult
{
    Equal,
    Less,
    Greater
};

ComparisonResult compare(double a, double b, double eps = EPS);

} // namespace Utils

#endif //UTILS_DOUBLES_HPP