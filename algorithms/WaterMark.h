#ifndef ALGORITHM_WATER_MARK_H_
#define ALGORITHM_WATER_MARK_H_

#include "WaterMarkTraits.h"

namespace pandora {

class WaterMark :
    virtual public noncopyable {

public:
    typedef WaterMark                       MyType;
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
    WaterMark(uint32_t argNum, va_list va);
    ~WaterMark();

private:
    bool     mInited;
    ModuleType mModule;
    bool     mPf;
    uint32_t mPfCnt;
    bool     mDump;
    AlgCaps  mCaps;
    WaterMarkParmType mParm;
    bool     mParmValid;

private:
    sp<IAlgorithm> mLogo;
    sp<IAlgorithm> mWaterMark;
    sp<IAlgorithm> mFont;
};

};

#endif


