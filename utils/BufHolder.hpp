#ifndef BUF_HOLDER_HPP_
#define BUF_HOLDER_HPP_

#include "BufHolder.h"

namespace pandora {

template <typename T>
BufHolder<T>::BufHolder() :
    mSize(0),
    mModule(MODULE_BUFFER_MANAGER)
{
}

template <typename T>
BufHolder<T>::~BufHolder()
{
    if (mBufs.size()) {
        clear();
    }
}

template <typename T>
BufHolder<T>::BufHolder(const BufHolder<T> &rhs)
{
    int32_t rc = NO_ERROR;

    for (auto &item : rhs.mBufs) {
        rc = add(item);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to add buf to holder");
        }
    }
}

template <typename T>
BufHolder<T> &BufHolder<T>::operator=(const BufHolder<T> &rhs)
{
    int32_t rc = NO_ERROR;

    if (this != &rhs) {
        if (size() > 0) {
            clear();
        }

        for (auto &item : rhs.mBufs) {
            rc = add(item);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to add buf to holder");
            }
        }
    }

    return *this;
}

template <typename T>
bool BufHolder<T>::operator==(const BufHolder<T> &rhs)
{
    bool result = true;

    if (size() != rhs.size()) {
        result = false;
    }

    if (result) {
        bool found = false;

        for (auto &l : mBufs) {
            for (auto &r : rhs.mBufs) {
                if (*l == *r) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                result = false;
                break;
            }
        }
    }

    return result;
}

template <typename T>
bool BufHolder<T>::operator!=(const BufHolder<T> &rhs)
{
    return !(*this == rhs);
}

template <typename T>
int32_t BufHolder<T>::add(typename AlgTraits<T>::TaskType &task, size_t allocsize)
{
    int32_t rc = NO_ERROR;

    allocsize = allocsize == 0 ? task.size : allocsize;

    sp<BufItem<T> > item = new BufItem<T>(task, allocsize);
    if (NOTNULL(item)) {
        mBufs.push_back(item);
        mSize++;
    } else {
        rc = NO_MEMORY;
    }

    return rc;
}

template <typename T>
int32_t BufHolder<T>::add(BufItem<T> &item)
{
    int32_t rc = NO_ERROR;

    sp<BufItem<T> > buf = new BufItem<T>(item);
    if (NOTNULL(buf)) {
        mBufs.push_back(buf);
        mSize++;
    } else {
        rc = NO_MEMORY;
    }

    return rc;
}

template <typename T>
int32_t BufHolder<T>::replace(uint32_t index,
    typename AlgTraits<T>::TaskType &task, size_t allocsize)
{
    int32_t rc = NO_ERROR;

    // Replace first not existed buf item is allowed.
    // Replace other not existed buf item is prohibited.

    if (index > mSize) {
        LOGE(mModule, "Invalid index %d/%d", index, mSize);
        rc = PARAM_INVALID;
    }

    if (SUCCEED(rc))  {
        allocsize = allocsize == 0 ? task.size : allocsize;
    }

    if (SUCCEED(rc)) {
        if (index == mSize) {
            rc = add(task, allocsize);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to add task to holder, %d", rc);
            } else {
                rc = NOT_INITED;
            }
        }
    }

    if (SUCCEED(rc)) {
        if (allocsize != mBufs[index]->replace(task, allocsize)) {
            LOGE(mModule, "Failed to replace buffer %d %d/%d bytes.",
                index, task.size, allocsize);
            rc = NO_MEMORY;
        }
    }

    return RETURNIGNORE(rc, NOT_INITED);
}

template <typename T>
int32_t BufHolder<T>::replace(uint32_t index, BufItem<T> &item)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        rc = remove(index);
    }

    if (SUCCEED(rc)) {
        mBufs.insert(mBufs.begin() + index, item);
    }

    return rc;
}

template <typename T>
int32_t BufHolder<T>::remove(uint32_t index)
{
    int32_t rc = NO_ERROR;

    if (index < size()) {
        mBufs.erase(mBufs.begin() + index);
        mSize--;
    } else {
        rc = NOT_INITED;
    }

    return rc;
}

template <typename T>
int32_t BufHolder<T>::remove(BufItem<T> &item)
{
    bool found = false;

    for (auto iter = mBufs.begin();
        iter != mBufs.end(); iter++) {
        if ((*iter).mBuf == item.mBuf) {
            found = true;
            mBufs.erase(iter);
            mSize--;
            break;
        }
    }

    return found ? NO_ERROR : NOT_INITED;
}

template <typename T>
void BufHolder<T>::clear()
{
    while(mBufs.begin() != mBufs.end()) {
        mBufs.erase(mBufs.begin());
    }
    mSize = 0;
}

template <typename T>
void *BufHolder<T>::operator[](uint32_t index)
{
    return (index < mSize) ? mBufs[index]->getBuf() : NULL;
}

template <typename T>
uint32_t BufHolder<T>::size()
{
    return mSize;
}

template <typename T>
void *BufHolder<T>::getLastData()
{
    return operator[](size() - 1);
}

template <typename T>
int32_t BufHolder<T>::getLastTask(typename AlgTraits<T>::TaskType &frame)
{
    frame = mBufs[size() - 1]->getTask();
    return NO_ERROR;
}

};

#endif
