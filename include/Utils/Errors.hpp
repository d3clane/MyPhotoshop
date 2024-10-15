#ifndef UTILS_ERRORS_HPP
#define UTILS_ERRORS_HPP

// TODO: split on simulator errors, engine errors, graphics errors, gui errors. 
// Move to another file in different namespaces

namespace Utils
{

enum class SimulatorErrors
{
    NoErr,
    InvalidBasisVectorsInCoordsSystem,
    InvalidSize,
    InvalidMass,
    InvalidSpeedAbs,
    ChemistryErr,
    PhysicsErr,
    LoadingFromFileErr,
    InvalidIterator,
};

} // namespace Utils

#endif // UTILS_ERRORS_HPP