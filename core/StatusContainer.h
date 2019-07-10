#ifndef _STATUS_CONTAINER_H_
#define _STATUS_CONTAINER_H_

#include <list>

#include "Common.h"
#include "RWLock.h"

namespace pandora {

class ContainerBase {
public:
    virtual uint32_t get_capacity();
    virtual uint32_t get_taken();
    virtual uint32_t get_available();
    virtual uint32_t size();
    virtual void dump();

public:
    ContainerBase(uint32_t capacity = 10);
    virtual ~ContainerBase() = default;

protected:
    uint32_t mTaken;
    uint32_t mCapacity;
};

template<typename T>
class InfoContainer :
    public ContainerBase {
public:
    explicit InfoContainer(uint32_t capacity = 10);
    ~InfoContainer() = default;
    uint32_t push_back(const T &dat);
    T &operator[](uint32_t id);
    bool operator==(const InfoContainer<T> &rhs);
    void dump() override;

private:
    T        mReserve;
    std::list<T> mList;
    RWLock   mListLock;
};

};

#include "StatusContainer.hpp"

#endif
