#include "BufItem.h"
#include "MemMgmt.h"

namespace pandora {

template <typename T>
BufItem<T>::BufItem(
    typename AlgTraits<T>::TaskType &task, size_t allocsize) :
    mBuf(NULL),
    mSize(0),
    mModule(MODULE_BUFFER_MANAGER),
    mTask(task)
{
    if (NOTNULL(mTask.data) && allocsize > 0) {
        replace(task, allocsize);
    } else {
        LOGE(mModule, "Failed to create buf item, buf %p size %d",
            mTask.data, allocsize);
    }
}

template <typename T>
size_t BufItem<T>::replace(
    typename AlgTraits<T>::TaskType &task, size_t allocsize)
{
    ASSERT_LOG(mModule, task.size <= allocsize,
        "Invalid data size %d and alloc size %d", task.size, allocsize);

    if (allocsize != mSize) {
        if (NOTNULL(mBuf)) {
            SECURE_FREE(mBuf);
        }
        mBuf = Malloc(allocsize);
        if (NOTNULL(mBuf)) {
            mSize = allocsize;
            memcpy(mBuf, task.data, task.size);
            mTask = task;
            mTask.data = mBuf;
        } else {
            LOGE(mModule, "Failed to create buf item %d bytes", allocsize);
        }
    } else {
        memcpy(mBuf, task.data, mSize);
        mTask = task;
        mTask.data = mBuf;
    }

    return mSize;
}

template <typename T>
BufItem<T>::~BufItem()
{
    if (NOTNULL(mBuf)) {
        SECURE_FREE(mBuf);
    }
}

template <typename T>
BufItem<T>::BufItem(const BufItem<T> &rhs)
{
    BufItem<T>(rhs.mTask);
}

template <typename T>
BufItem<T> &BufItem<T>::operator=(const BufItem<T> &rhs)
{
    if (this != &rhs) {
        if (NOTNULL(mBuf)) {
            SECURE_FREE(mBuf);
        }

        void *buf = rhs.getBuf();
        size_t size = rhs.getSize();
        if (NOTNULL(buf) && size > 0) {
            mBuf = Malloc(size);
            if (NOTNULL(mBuf)) {
                mSize = size;
                memcpy(mBuf, buf, size);
                mTask = rhs.mTask;
                mTask.data = mBuf;
            } else {
                LOGE(mModule, "Failed to create buf item %d bytes", size);
            }
        } else {
            LOGE(mModule, "Failed to create buf item, buf %p size %d",
                buf, size);
        }
    }

    return *this;
}

template <typename T>
bool BufItem<T>::operator==(const BufItem<T> &rhs) const
{
    bool rc = false;
    if ((rhs.getSize() == mSize) &&
        (!memcmp(rhs.getBuf(), mBuf, rhs.getSize()))) {
        rc = true;
    }

    return rc;
}

template <typename T>
bool BufItem<T>::operator!=(const BufItem<T> &rhs) const
{
    return !(*this == rhs);
}

template <typename T>
void *BufItem<T>::getBuf() const
{
    return mBuf;
}

template <typename T>
typename AlgTraits<T>::TaskType BufItem<T>::getTask() const
{
    return mTask;
}

template <typename T>
size_t BufItem<T>::getSize() const
{
    return mSize;
}

};

