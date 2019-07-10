#include "FrameQueue.h"

namespace pandora {

template <typename T>
FrameQueue<T>::FrameQueue(uint32_t capacity) :
    mCap(capacity),
    mModule(MODULE_UTILS)
{
    ASSERT_LOG(mModule, capacity > 0, "capacity invalid %d", capacity);
}

template <typename T>
FrameQueue<T>::~FrameQueue()
{
    clear();
}

template <typename T>
void FrameQueue<T>::clear()
{
    mBufs.clear();
    mTask.clear();
}

template <typename T>
int32_t FrameQueue<T>::add(typename AlgTraits<T>::TaskType task)
{
    return push_back(task);
}

template <typename T>
int32_t FrameQueue<T>::push_back(typename AlgTraits<T>::TaskType task)
{
    int32_t rc = NO_ERROR;

    rc = mBufs.add(task);
    if (!SUCCEED(rc)) {
        LOGE(mModule, "Failed to add buffers, %d", rc);
    }

    if (SUCCEED(rc)) {
        task.data = mBufs[mBufs.size() - 1];
        mTask.push_back(task);
    }

    if (SUCCEED(rc)) {
        while (mBufs.size() > mCap) {
            rc = mBufs.remove(0);
            mTask.erase(mTask.begin());
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to remove buf 0, %d", rc);
            }
        }
    }

    return rc;
}

template <typename T>
int32_t FrameQueue<T>::replace(int32_t index,
    typename AlgTraits<T>::TaskType task)
{
    int32_t rc = NO_ERROR;

    rc = mBufs.replace(index, task);
    if (!SUCCEED(rc)) {
        LOGE(mModule, "Failed to add buffers, %d", rc);
    }

    if (SUCCEED(rc)) {
        task.data = mBufs[index];
        mTask.push_back(task);
    }

    if (SUCCEED(rc)) {
        while (mTask.size() > mCap) {
            mTask.erase(mTask.begin());
        }
    }

    return rc;
}

template <typename T>
uint32_t FrameQueue<T>::size()
{
    return mBufs.size();
}

template <typename T>
typename AlgTraits<T>::TaskType FrameQueue<T>::getTask(uint32_t index)
{
    ASSERT_LOG(mModule, index < mBufs.size(),
        "index invalid %d/%d", index, mBufs.size());

    return *(mTask.begin() + index);
}

template <typename T>
void *FrameQueue<T>::getData(uint32_t index)
{
    ASSERT_LOG(mModule, index < mBufs.size(),
        "index invalid %d/%d", index, mBufs.size());

    return mBufs[index];
}

};

