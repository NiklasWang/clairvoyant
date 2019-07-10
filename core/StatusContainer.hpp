#ifndef _STATUS_CONTAINER_HPP_
#define _STATUS_CONTAINER_HPP_

#include "StatusContainer.h"

namespace pandora {

template<typename T>
InfoContainer<T>::InfoContainer(uint32_t capacity) :
    ContainerBase(capacity)
{
}

template<typename T>
uint32_t InfoContainer<T>::push_back(const T &dat)
{
    RWLock::AutoWLock l(mListLock);

    mList.push_back(dat);
    mTaken = mList.size();

    if (mTaken > mCapacity) {
        auto iter = mList.begin();
        mList.erase(iter);
    }
    mTaken = mList.size();

    return NO_ERROR;
}

template<typename T>
T &InfoContainer<T>::operator[](uint32_t id)
{
    uint32_t idx = 0;
    T *result = NULL;

    RWLock::AutoRLock l(mListLock);
    for (auto &dat : mList) {
        if (idx++ == id) {
            result = &dat;
            break;
        }
    }

    if (ISNULL(result)) {
        result = &mReserve;
    }

    return *result;
}

template<typename T>
bool InfoContainer<T>::operator==(const InfoContainer<T> &rhs)
{
    bool result = false;

    if (mList.size() == rhs.mList.size()) {
        auto iter1 = mList.begin();
        auto iter2 = rhs.mList.begin();
        while(iter1 != mList.end() &&
            iter2 != rhs.mList.end()) {
            if (*iter1 != *iter2) {
                break;
            }
        }
        if (iter1 == mList.end() &&
            iter2 == rhs.mList.end()) {
            result = true;
        }
    }

    return result;
}

template<typename T>
void InfoContainer<T>::dump()
{
    RWLock::AutoRLock l(mListLock);
    for (auto &item : mList) {
        item.dump();
    }
}

};

#endif
