#ifndef _ALGORITHM_BMP_WATER_MARK_H_
#define _ALGORITHM_BMP_WATER_MARK_H_

#include "BmpWaterMarkTraits.h"

namespace pandora {

class BmpWaterMark :
    virtual public noncopyable {

public:
    typedef BmpWaterMark                    MyType;
    typedef AlgTraits<MyType>::TaskType     TaskTypeT;
    typedef AlgTraits<MyType>::ResultType   ResultTypeT;
    typedef AlgTraits<MyType>::UpdateType   UpdateTypeT;
    typedef AlgTraits<MyType>::ParmType     ParmTypeT;
    typedef AlgTraits<MyType>::RequestType  RequestTypeT;
    typedef AlgTraits<MyType>::StatusType   StatusTypeT;

    AlgType getType() { return ALG_UTILS; }
    int32_t init();
    int32_t deinit();
    int32_t process(TaskTypeT &task, ResultTypeT &result);
    int32_t process(TaskTypeT &in, TaskTypeT &out, ResultTypeT &result);
    int32_t setParm(ParmTypeT &parm);
    int32_t update(UpdateTypeT &update);
    int32_t queryCaps(AlgCaps &caps);
    int32_t queryStatus(StatusTypeT &status);

public:
    BmpWaterMark(uint32_t argNum, va_list va);
    ~BmpWaterMark();

private:
    bool     mInited;
    ModuleType mModule;
    bool     mPf;
    uint32_t mPfCnt;
    bool     mDump;
    AlgCaps  mCaps;

    uint8_t   *mBmp;
    uint32_t   mSize; /* 0% - 100%, default 5% */
    uint8_t   *mUnscaledYuv;
    uint8_t   *mUnscaledMirrorYuv;
    FrameInfo *mUnscaledFrame;
    sp<IAlgorithm> mScaler;
    sp<IAlgorithm> mRotator;
    uint8_t   *mScaledYuv;
    FrameInfo *mScaledFrame;
    uint8_t   *mRotatedYuv;
    FrameInfo *mRotatedFrame;
    bool       mParmValid;
    BmpWaterMarkParmType mParm;
    RotationAngle        mRotation;
    BmpWaterMarkPosition mPosition;
    RotationAngle        mLastRotation;
};

};

#endif

