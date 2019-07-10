#include "StatusContainer.h"

namespace pandora {

ContainerBase::ContainerBase(uint32_t capacity) :
    mTaken(0),
    mCapacity(capacity)
{
}

uint32_t ContainerBase::get_capacity()
{
    return mCapacity;
}

uint32_t ContainerBase::get_taken()
{
    return mTaken;
}

uint32_t ContainerBase::size()
{
    return get_taken();
}

uint32_t ContainerBase::get_available()
{
    return get_capacity() - get_taken();
}

void ContainerBase::dump()
{
}

};
