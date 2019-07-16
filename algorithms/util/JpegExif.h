#ifndef ALGORITHM_JPEG_EXIF_H_
#define ALGORITHM_JPEG_EXIF_H_

#include "JpegExifTraits.h"
#include "SemaphoreTimeout.h"

namespace pandora {

class JpegExif :
    virtual public noncopyable {

public:
    typedef JpegExif                        MyType;
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
    JpegExif(uint32_t argNum, va_list va);
    ~JpegExif();

public:
    int32_t waitAndReadExif(uint8_t **exifData, uint32_t *exifSize);

private:
    bool       mInited;
    AlgCaps    mCaps;
    ModuleType mModule;
    uint8_t   *mExifBuf;
    uint32_t   mExifSize;
    uint32_t   mBufSize;
    SemaphoreTimeout mSem;
};

};

#endif


