#ifndef _ALGORITHM_INTERFACE_
#define _ALGORITHM_INTERFACE_

#include <functional>

#include "Common.h"
#include "AlgorithmType.h"
#include "GlobalTraits.h"
#include "RefBase.h"
#include "SyncType.h"

namespace pandora {

typedef AlgTraits<Global>::TaskType    TaskType;
typedef AlgTraits<Global>::UpdateType  UpdateType;
typedef AlgTraits<Global>::ParmType    ParmType;
typedef AlgTraits<Global>::ResultType  ResultType;
typedef AlgTraits<Global>::RequestType RequestType;
typedef AlgTraits<Global>::StatusType  StatusType;

class IAlgListener;

struct AlgCaps {
    uint32_t frameNeeded;
    uint32_t workingBufCnt;
    size_t   workingBufSize;
    FrameTypeMaskType   types;
    FrameFormatMaskType formats;
};

class IAlgorithm :
    public RefBase {
public:
    virtual int32_t process(TaskType &task,
        uint32_t taskId = 0, SyncTypeE sync = SYNC_TYPE,
        std::function<int32_t (TaskType &)> cb = [](TaskType &) -> int32_t { return 0; }) = 0;
    virtual int32_t process(TaskType &task, TaskType &result,
        uint32_t taskId = 0, SyncTypeE sync = SYNC_TYPE,
        std::function<int32_t (TaskType &)> cb = [](TaskType &) -> int32_t { return 0; }) = 0;

    virtual int32_t processSync(TaskType &task, uint32_t taskId = 0);
    virtual int32_t processAsync(TaskType &task, uint32_t taskId = 0,
        std::function<int32_t (TaskType &)> cb = [](TaskType &) -> int32_t { return 0; });

    virtual int32_t processSync(TaskType &task, TaskType &result, uint32_t taskId = 0);
    virtual int32_t processAsync(TaskType &task, TaskType &result, uint32_t taskId = 0,
        std::function<int32_t (TaskType &)> cb = [](TaskType &) -> int32_t { return 0; });

    virtual int32_t config(ParmType &parm, SyncTypeE sync = SYNC_TYPE) = 0;
    virtual int32_t configSync(ParmType &parm);
    virtual int32_t configAsync(ParmType &parm);
    virtual int32_t update(UpdateType &update, SyncTypeE sync = ASYNC_TYPE) = 0;
    virtual int32_t updateSync(UpdateType &update);
    virtual int32_t updateAsync(UpdateType &update);

    virtual int32_t queryStatus(StatusType &status) = 0;
    virtual AlgCaps queryCap() const = 0;

    virtual const char *getName() const = 0;
    virtual const char *whoamI() const;
    virtual AlgType getType() const = 0;
    virtual AlgType myType() const;
    virtual bool busy() const = 0;
    virtual bool idle() const;

    virtual int32_t addListener(pthread_t tid, IAlgListener *listener) = 0;
    virtual int32_t removeListener(IAlgListener *listener) = 0;
    virtual int32_t setLinker(IdType parent,
        sp<IAlgorithm> prev, sp<IAlgorithm> next) = 0;
    virtual ~IAlgorithm() = default;
};

template<typename T>
class IAlgorithmT :
    public IAlgorithm {
public:
    virtual T *getAlgorithmInDanger() { return NULL; };
};

class IAlgListener {
public:
    virtual int32_t onAlgResultAvailable(int32_t taskId, ResultType *result) = 0;
    virtual ~IAlgListener() = default;
};

class IAlgRequestHandler {
public:
    virtual int32_t onAlgRequest(RequestType *request, AlgType submitter) = 0;
    virtual ~IAlgRequestHandler() = default;
};

};

#endif

