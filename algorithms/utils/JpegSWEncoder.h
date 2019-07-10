#ifndef ALGORITHM_JPEG_SW_ENCODER_H_
#define ALGORITHM_JPEG_SW_ENCODER_H_

#include "JpegSWEncoderTraits.h"

namespace pandora {

class JpegSWEncoder :
    virtual public noncopyable {

public:
    typedef JpegSWEncoder                   MyType;
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
    JpegSWEncoder(uint32_t argNum, va_list va);
    ~JpegSWEncoder();

private:
    bool       mInited;
    bool       mDump;
    bool       mPf;
    uint32_t   mPfCnt;
    AlgCaps    mCaps;
    ModuleType mModule;
    void      *mEngine;
    uint8_t   *mJpegBuf;
    uint8_t   *mYuv420pBuf;
    uint32_t   mJpegBufSize;
    uint32_t   mPreBufSize;
    uint32_t   mPreYuvSize;
};

};

#endif


