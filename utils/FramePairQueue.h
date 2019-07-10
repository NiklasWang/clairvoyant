#ifndef _FRAME_PAIR_QUEUE_H_
#define _FRAME_PAIR_QUEUE_H_

#include "FrameQueue.h"

#define SAME_TIMESTAMP_THRES 10*1e6

namespace pandora {

template <typename T>
class FramePairQueue {
public:
    int32_t add(typename AlgTraits<T>::TaskType &task);
    int32_t enqueue(typename AlgTraits<T>::TaskType &task);
    int32_t replace(int32_t index,
        typename AlgTraits<T>::TaskType &task);
    int32_t getFrame(int32_t id, FrameFormat format, typename AlgTraits<T>::TaskType &frame);
    int32_t getLastFrame(typename AlgTraits<T>::TaskType &color,
        typename AlgTraits<T>::TaskType &sub);
    int32_t getNearestFrame(typename AlgTraits<T>::TaskType &color,
        typename AlgTraits<T>::TaskType &sub);
    void    clear();
    uint32_t size(FrameFormat format);
    int32_t getLastTask(FrameFormat format, typename AlgTraits<T>::TaskType &frame);

public:
    FramePairQueue(uint32_t capacity);

private:
    bool SameTimestamp(typename AlgTraits<T>::TaskType &color,
        typename AlgTraits<T>::TaskType &sub);

private:
    ModuleType    mModule;
    FrameQueue<T> mColorQ;
    FrameQueue<T> mMonoQ;
};

};

#include "FramePairQueue.hpp"

#endif
