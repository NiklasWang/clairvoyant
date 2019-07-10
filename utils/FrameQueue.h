#ifndef _BUFFER_QUEUE_H_
#define _BUFFER_QUEUE_H_

#include "BufHolder.h"

namespace pandora {

template <typename T>
class FrameQueue {
public:
    int32_t add(typename AlgTraits<T>::TaskType task);
    int32_t push_back(typename AlgTraits<T>::TaskType task);
    int32_t replace(int32_t index, typename AlgTraits<T>::TaskType task);
    typename AlgTraits<T>::TaskType getTask(uint32_t index);
    void    *getData(uint32_t index);
    uint32_t size();
    void     clear();

public:
    FrameQueue(uint32_t capacity);
    ~FrameQueue();

private:
    uint32_t     mCap;
    ModuleType   mModule;
    BufHolder<T> mBufs;
    std::vector<typename AlgTraits<T>::TaskType> mTask;
};

};

#include "FrameQueue.hpp"

#endif
