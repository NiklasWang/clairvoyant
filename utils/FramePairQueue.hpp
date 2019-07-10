#include "FramePairQueue.h"
#include "FrameQueue.h"

namespace pandora {

template <typename T>
int32_t FramePairQueue<T>::add(typename AlgTraits<T>::TaskType &task)
{
    return enqueue(task);
}

template <typename T>
int32_t FramePairQueue<T>::enqueue(typename AlgTraits<T>::TaskType &task)
{
    int32_t rc = NO_ERROR;

    if (task.format == FRAME_FORMAT_YUV_MONO) {
        rc = mMonoQ.push_back(task);
    } else {
        rc = mColorQ.push_back(task);
    }

    if (!SUCCEED(rc)) {
        LOGE(mModule, "Failed to enqueue, %d", rc);
    }

    return rc;
}

template <typename T>
int32_t FramePairQueue<T>::replace(int32_t index,
    typename AlgTraits<T>::TaskType &task)
{
    int32_t rc = NO_ERROR;

    if (task.format == FRAME_FORMAT_YUV_MONO) {
        rc = mMonoQ.replace(index, task);
    } else {
        rc = mColorQ.replace(index, task);
    }

    if (!SUCCEED(rc)) {
        LOGE(mModule, "Failed to enqueue, %d", rc);
    }

    return rc;
}

template <typename T>
int32_t FramePairQueue<T>::getFrame(int32_t index,
    FrameFormat format, typename AlgTraits<T>::TaskType &frame)
{
    int32_t rc = NO_ERROR;
    int32_t size = format == FRAME_FORMAT_YUV_MONO ?
        mMonoQ.size() : mColorQ.size();

    if (index > size) {
        LOGE(MODULE_UTILS, "Invalid index %d/%d, frame type %d",
            index, size, format);
        rc = PARAM_INVALID;
    }

    if (format == FRAME_FORMAT_YUV_MONO) {
        frame = mMonoQ.getTask(index);
    } else {
        frame = mColorQ.getTask(index);
    }

    return rc;
}

template <typename T>
bool FramePairQueue<T>::SameTimestamp(
    typename AlgTraits<T>::TaskType &color,
    typename AlgTraits<T>::TaskType &sub)
{
    bool result = false;

    if (llabs(color.ts - sub.ts) < SAME_TIMESTAMP_THRES) {
        result = true;
    }

    return result;
}


template <typename T>
int32_t FramePairQueue<T>::getLastFrame(
    typename AlgTraits<T>::TaskType &colorFrame,
    typename AlgTraits<T>::TaskType &monoFrame)
{
    int32_t rc = NO_ERROR;
    bool found = false;
    int32_t i = mColorQ.size();
    int32_t j = mMonoQ.size();

    while (--i >= 0) {
        typename AlgTraits<T>::TaskType ctask = mColorQ.getTask(i);
        j = mMonoQ.size();
        while (--j >= 0) {
            typename AlgTraits<T>::TaskType mtask = mMonoQ.getTask(j);
            if (SameTimestamp(ctask, mtask)) {
                found = true;
                break;
            }
        }

        if (found) {
            break;
        }
    }

    if (!found) {
        rc = NOT_INITED;
        LOGW(mModule, "Didn't found matched color/sub frame.");
    } else {
        colorFrame = mColorQ.getTask(i);
        monoFrame = mMonoQ.getTask(j);
    }

    return rc;
}

template <typename T>
int32_t FramePairQueue<T>::getNearestFrame(
    typename AlgTraits<T>::TaskType &colorFrame,
    typename AlgTraits<T>::TaskType &monoFrame)
{
    int32_t rc = NO_ERROR;
    int32_t i = 0, j = 0, found_index = 0;
    int64_t offsetTime = 0, miniDeta=0;

    int32_t colorSize = mColorQ.size();
    int32_t monoSize = mMonoQ.size();

    if (colorSize == 0 || monoSize == 0) {
        rc = NOT_INITED;
    }

    if (SUCCEED(rc)) {
        i = colorSize - 1;
        j = monoSize - 1;
        found_index = j;
        typename AlgTraits<T>::TaskType ctask = mColorQ.getTask(i);
        typename AlgTraits<T>::TaskType mtask = mMonoQ.getTask(j);
        miniDeta = llabs(ctask.ts - mtask.ts);
        while (--j >= 0) {
            mtask = mMonoQ.getTask(j);
            offsetTime = llabs(ctask.ts - mtask.ts);
            if (offsetTime < miniDeta) {
                miniDeta = offsetTime;
                found_index = j;
            }
        }

        colorFrame = mColorQ.getTask(i);
        monoFrame = mMonoQ.getTask(found_index);
    }

    return rc;
}

template <typename T>
FramePairQueue<T>::FramePairQueue(uint32_t capacity) :
    mModule(MODULE_UTILS),
    mColorQ(capacity),
    mMonoQ(capacity)
{
    ASSERT_LOG(mModule, capacity > 0, "capacity invalid %d", capacity);
}

template <typename T>
void FramePairQueue<T>::clear()
{
    mColorQ.clear();
    mMonoQ.clear();
}

template <typename T>
uint32_t FramePairQueue<T>::size(FrameFormat format)
{
    return format == FRAME_FORMAT_YUV_MONO ?
        mMonoQ.size() : mColorQ.size();
}

template <typename T>
int32_t FramePairQueue<T>::getLastTask(FrameFormat format,
    typename AlgTraits<T>::TaskType &frame)
{
    return getFrame(size(format) - 1, format, frame);
}

};

