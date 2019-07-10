#ifndef ALGORITHM_ROTATE_YUV_H_
#define ALGORITHM_ROTATE_YUV_H_

#include "YUVRotatorTraits.h"
#include "BufHolder.h"

namespace pandora {

class YUVRotator :
    virtual public noncopyable {

public:
    typedef YUVRotator                      MyType;
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
    YUVRotator(uint32_t argNum, va_list va);
    ~YUVRotator();

private:
    bool     mInited;
    ModuleType mModule;
    bool     mPf;
    uint32_t mPfCnt;
    bool     mDump;
    AlgCaps  mCaps;
    ParmTypeT mParms;
    bool     mParmValid;
    BufHolder<MyType> mBuf;
};

};

#endif


