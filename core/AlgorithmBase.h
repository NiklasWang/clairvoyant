#ifndef ALGORITHM_BASE_H_
#define ALGORITHM_BASE_H_

#include <stdarg.h>
#include <list>
#include <functional>

#include "AlgorithmIntf.h"
#include "ThreadT.h"
#include "SyncType.h"
#include "GlobalTraits.h"
#include "ObjectBuffer.h"
#include "RefBase.h"

namespace pandora {

template <typename T, typename Trait = AlgTraits<T> >
class AlgorithmBase :
    public IAlgorithmT<T>,
    public Identifier
{
public:
    int32_t process(TaskType &task,
        uint32_t taskId = 0, SyncTypeE sync = SYNC_TYPE,
        std::function<int32_t (TaskType &)> cb = [](TaskType &) -> int32_t { return 0; }) override;
    int32_t process(TaskType &task, TaskType &result,
        uint32_t taskId = 0, SyncTypeE sync = SYNC_TYPE,
        std::function<int32_t (TaskType &)> cb = [](TaskType &) -> int32_t { return 0; }) override;
    int32_t config(ParmType &parm, SyncTypeE sync = SYNC_TYPE) override;
    int32_t update(UpdateType &update, SyncTypeE sync = ASYNC_TYPE) override;
    int32_t queryStatus(StatusType &status) override;

    const char *getName() const override;
    AlgType getType() const override;
    bool busy() const override;
    AlgCaps queryCap() const override;

    T *getAlgorithmInDanger() override;

    int32_t addListener(pthread_t tid, IAlgListener *listener) override;
    int32_t removeListener(IAlgListener *listener) override;
    int32_t setLinker(IdType parent,
        sp<IAlgorithm> prev, sp<IAlgorithm> next) override;

public:
    typedef T                           AlgorithmType;
    typedef typename Trait::TaskType    TaskTypeT;
    typedef typename Trait::ResultType  ResultTypeT;
    typedef typename Trait::UpdateType  UpdateTypeT;
    typedef typename Trait::ParmType    ParmTypeT;
    typedef typename Trait::RequestType RequestTypeT;
    typedef typename Trait::StatusType  StatusTypeT;
    typedef AlgorithmBase<T, Trait>     MyType;

private:
    virtual int32_t initAlgorithm() = 0;
    virtual int32_t deinitAlgorithm() = 0;
    virtual int32_t processNewTask(TaskTypeT &task) = 0;
    virtual int32_t processNewTask(TaskTypeT &in, TaskTypeT &out) = 0;
    virtual int32_t processUpdate(UpdateTypeT &update) = 0;
    virtual int32_t processParms(ParmTypeT &parm) = 0;
    virtual int32_t processQueryStatus(StatusTypeT &status) = 0;
    virtual AlgorithmType *getAlgorithmInDangerInternal() = 0;


protected:
    int32_t construct();
    int32_t destruct();
    int32_t reset();

private:
    struct Listener;
    Listener *traverseListenerList(Listener *cur);
    int32_t notifyListener();
    struct Linker;
    Linker *traverseLinkerList(Linker *cur);
    int32_t broadcastUpdate(AlgTraits<Global>::UpdateType update);
    struct AlgNewTask;
    int32_t process(AlgNewTask *task);
    struct AlgNewParm;
    int32_t config(AlgNewParm *parm);
    struct AlgNewUpdate;
    int32_t update(AlgNewUpdate *update);
    int32_t queryStatus(StatusType *status);
    struct AlgEvtInf;
    int32_t processTask(AlgEvtInf *evt);
    int32_t taskDone(AlgEvtInf *evt, int32_t _rc);

private:
    enum AlgEvt {
        ALG_EVT_NEW_TASK = 0,
        ALG_EVT_UPDATE = 1,
        ALG_EVT_SET_PARM = 2,
        ALG_EVT_QUERY_STATUS = 3,
        ALG_EVT_INVALID_MAX = 4,
    };

    struct AlgNewTask {
        TaskTypeT  info;
        TaskTypeT *result;
        uint32_t   taskId;
        SyncTypeE  sync;
        bool       buffered;
        std::function<int32_t (TaskType &)> cb;
    };

    struct AlgNewUpdate {
        UpdateTypeT info;
        bool        buffered;
    };

    struct AlgNewParm {
        ParmTypeT info;
        bool      buffered;
    };

    struct AlgEvtInf {
    public:
        uint32_t  id;
        AlgEvt    type;
        bool      buffered;
        void     *job;

    public:
        void clear();
        static const char * const kEvtTypeString[];
        const char *whoamI();
    };

    template <AlgEvt type, SyncType::value_type sync = SYNC_TYPE>
    int32_t queueToThread(void *arg);
    typedef int32_t (MyType::*queueToThreadF)(void *data);

protected:
    AlgorithmBase(std::string &algName,
        std::string &threadName, bool frameDrop, uint32_t maxQueueSize);
    virtual ~AlgorithmBase();

private:
    struct Listener {
        pthread_t tid;
        IAlgListener *listener;
    };

    struct Task {
        int32_t   id;
        TaskTypeT task;
    };

    struct Linker {
        IdType         parent;
        wp<IAlgorithm> prev;
        std::string    prevName;
        wp<IAlgorithm> next;
        std::string    nextName;
    };

    template<typename X>
    struct ObjectBufferEx :
        public ObjectBuffer<X>,
        public RefBase {
    };

protected:
    bool     mConstructed;
    AlgType  mAlgType;
    AlgCaps  mCaps;
    ResultTypeT mResult;

private:
    bool       mFrameDrop;
    uint32_t   mMaxQueueSize;
    std::list<Listener> mListener;
    RWLock     mListenerLock;
    Task       mTask;
    uint32_t   mTaskNum;
    int32_t    mUnFishedTaskNum;
    uint32_t   mUpdateNum;
    uint32_t   mParmNum;
    uint32_t   mStatusNum;
    uint32_t   mEvtNum;
    RWLock     mLinkerLock;
    sp<ObjectBufferEx<AlgNewTask> >   mTaskObj;
    sp<ObjectBufferEx<AlgNewParm> >   mParmObj;
    sp<ObjectBufferEx<AlgNewUpdate> > mUpdateObj;
    sp<ObjectBufferEx<AlgEvtInf> >    mEvtObj;
    ThreadT<AlgEvtInf> mThread;
    std::list<Linker>  mLinker;
    static const queueToThreadF kQueuedThreadFunc[][SYNC_MAX_INVALID];
};

};

#include "AlgorithmBase.hpp"

#endif
