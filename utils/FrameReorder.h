#ifndef _FRAME_RESCHEDULER_H_
#define _FRAME_RESCHEDULER_H_

#include "Modules.h"
#include "SemaphoreTimeout.h"
#include "PandoraInterface.h"

namespace pandora {

template<typename K, typename V>
struct ParmMap {
    K key;
    V input1;
    V input2;
    int32_t inputTimeout; // ms
    V output1;
    V output2;
    int32_t outputTimeout;
};

class FrameReorder {
public:
    int32_t wait(FrameType type, FrameFormat format);
    int32_t signal(FrameType type, FrameFormat format);
    int32_t reorder(FrameType type, FrameFormat format);
    int32_t reset(FrameType type);
    void reset();
    int32_t start(FrameType type);
    int32_t stop(FrameType type);

public:
    static FrameReorder *getInstance();
    static uint32_t removeInstance();

private:
    FrameReorder();
    virtual ~FrameReorder();
    FrameReorder(const FrameReorder &rhs);
    FrameReorder &operator =(const FrameReorder &rhs);

private:
    void reset(SemaphoreTimeout *sem);

private:
    static const ModuleType mModule;
    static const ParmMap<FrameType, FrameFormat> gOrders[];
    bool               mStoped[FRAME_TYPE_MAX_INVALID];
    SemaphoreTimeout **mOrderSems;
    SemaphoreTimeout **mWaitSems;

private:
    static uint32_t gCnt;
    static pthread_mutex_t gInsL;
    static FrameReorder   *gThis;
};

};

#endif
