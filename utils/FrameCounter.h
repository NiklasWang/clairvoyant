#ifndef _FRAME_COUNTER_H_
#define _FRAME_COUNTER_H_

#include <pthread.h>
#include "Modules.h"
#include "PandoraInterface.h"

namespace pandora {

enum CameraMode {
    SingleCameraMode,
    DualCameraMode,
};

struct SingleCameraCounter {
    int32_t mFrameCounter;
    int32_t mTotalNum;
};

struct DualCameraCounter {
    bool    mMonoArrived;
    bool    mColorArrived;
    int32_t mPairCounter;
    int32_t mTotalPairs;
};

class FrameCounter {
public:
    FrameCounter();
    ~FrameCounter();
    int32_t reset();
    int32_t countFrame(FrameType type, FrameFormat format, bool *isTargetFrame);
    int32_t getCounter();
    bool    checkTarget();
    int32_t setTotalNum(int32_t num);
    int32_t setCameraMode(CameraMode mode);

private:
    ModuleType mModule;
    CameraMode mWorkMode;
    pthread_mutex_t mMutex;

    union Counter {
        DualCameraCounter   mDualCounter;
        SingleCameraCounter mSingleCounter;
    } mCounter;
};

};
#endif
