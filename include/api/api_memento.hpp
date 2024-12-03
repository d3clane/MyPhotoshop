#ifndef API_MEMENTO_HPP
#define API_MEMENTO_HPP

#include <memory>

namespace psapi
{


template<typename SnapshotType>
class IMementable
{
public:
    virtual ~IMementable() = default;

    virtual std::unique_ptr<SnapshotType> save() = 0;
    virtual void restore(SnapshotType* snapshot) = 0;
};

class ILayerSnapshot
{
public:
    virtual ~ILayerSnapshot() = default;
};

class ICanvasSnapshot
{
public:
    virtual ~ICanvasSnapshot() = default;
};


}

#endif // API_MEMENTO_HPP
