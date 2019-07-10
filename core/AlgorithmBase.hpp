#ifndef _ALGORITHMBASE_HPP_
#define _ALGORITHMBASE_HPP_

#include "AlgorithmBase.h"
#include "Atomic.h"

namespace pandora {

#define CHECK_DESTRUCTION() \
    ({ \
        int32_t __rc = NO_ERROR; \
        if (mConstructed == false) { \
            __rc = NOT_INITED; \
            LOGE(mModule, "Warning: Algorithm destructed."); \
            LOGE(mModule, "Destructed algorithm name is %s.", myName()); \
        } \
        __rc; \
    })

template <typename T, typename Trait>
AlgorithmBase<T, Trait>::AlgorithmBase(
    std::string &algName, std::string &threadName,
    bool frameDrop, uint32_t maxQueueSize) :
    Identifier(MODULE_ALGORITHM_BASE_TEMPLATE, algName.c_str()),
    mConstructed(false),
    mFrameDrop(frameDrop),
    mMaxQueueSize(maxQueueSize),
    mTaskNum(0),
    mUnFishedTaskNum(0),
    mUpdateNum(0),
    mParmNum(0),
    mStatusNum(0),
    mEvtNum(0),
    mTaskObj(NULL),
    mParmObj(NULL),
    mUpdateObj(NULL),
    mEvtObj(NULL),
    mThread(threadName)
{
}

template <typename T, typename Trait>
AlgorithmBase<T, Trait>::~AlgorithmBase()
{
    if (mConstructed) {
        destruct();
    }
}

template <typename T, typename Trait>
int32_t AlgorithmBase<T, Trait>::construct()
{
    int32_t rc = NO_ERROR;

    if (mConstructed) {
        rc = ALREADY_EXISTS;
    }

    if (SUCCEED(rc)) {
        rc = initAlgorithm();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to init algorithm %s", whoamI());
        }
    }

    if (SUCCEED(rc)) {
        if (ISNULL(mTaskObj)) {
            mTaskObj = new ObjectBufferEx<AlgNewTask>();
            if (ISNULL(mTaskObj)) {
                LOGE(mModule, "Failed to create task buffer");
                rc = NO_MEMORY;
            }
        }
        if (ISNULL(mParmObj)) {
            mParmObj = new ObjectBufferEx<AlgNewParm>();
            if (ISNULL(mParmObj)) {
                LOGE(mModule, "Failed to create parm buffer");
                rc = NO_MEMORY;
            }
        }
        if (ISNULL(mUpdateObj)) {
            mUpdateObj = new ObjectBufferEx<AlgNewUpdate>();
            if (ISNULL(mUpdateObj)) {
                LOGE(mModule, "Failed to create update buffer");
                rc = UNKNOWN_ERROR;
            }
        }
        if (ISNULL(mEvtObj)) {
            mEvtObj = new ObjectBufferEx<AlgEvtInf>();
            if (ISNULL(mEvtObj)) {
                LOGE(mModule, "Failed to create event buffer");
                rc = UNKNOWN_ERROR;
            }
        }
    }

    if (SUCCEED(rc)) {
        rc = mThread.construct();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to construct thread");
            deinitAlgorithm();
        }
    }

    if (SUCCEED(rc)) {
        mConstructed = true;
        LOGD(mModule, "Algorithm %s id %d constructed", whoamI(), id());
    } else {
        LOGE(mModule, "Failed to construct algorithm %s id %d", whoamI(), id());
    }

    return rc;
}

template <typename T, typename Trait>
int32_t AlgorithmBase<T, Trait>::destruct()
{
    int32_t rc = NO_ERROR;

    if (!mConstructed) {
        rc = NOT_INITED;
    } else {
        mConstructed = false;
    }

    if (SUCCEED(rc)) {
        rc = mThread.destruct();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to destruct thread");
        }
    }

    if (SUCCEED(rc)) {
        rc = deinitAlgorithm();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to deinit algorithm %s id %d", whoamI(), id());
        }
    }

    if (SUCCEED(rc)) {
        if (NOTNULL(mTaskObj)) {
            mTaskObj->flush();
        }
        if (NOTNULL(mParmObj)) {
            mParmObj->flush();
        }
        if (NOTNULL(mUpdateObj)) {
            mUpdateObj->flush();
        }
        if (NOTNULL(mEvtObj)) {
            mEvtObj->flush();
        }
    }

    if (!SUCCEED(rc)) {
        LOGE(mModule, "Failed to destructed algorithm %s id %d ", whoamI(), id());
    } else {
        LOGD(mModule, "Algorithm %s id %d destructed", whoamI(), id());
    }

    return rc;
}

template <typename T, typename Trait>
int32_t AlgorithmBase<T, Trait>::reset()
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        rc = destruct();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to destruct");
        }
    }

    if (SUCCEED(rc)) {
        rc = construct();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to construct");
        }
    }

    return rc;
}

template <typename T, typename Trait>
int32_t AlgorithmBase<T, Trait>::addListener(pthread_t tid,
    IAlgListener *aListener)
{
    int32_t rc = CHECK_DESTRUCTION();

    if (SUCCEED(rc)) {
        if (!ISNULL(aListener)) {
            Listener listener = { tid, aListener };
            RWLock::AutoWLock l(mListenerLock);
            mListener.push_back(listener);
        }
    }

    return rc;
}

template <typename T, typename Trait>
int32_t AlgorithmBase<T, Trait>::removeListener(IAlgListener *listener)
{
    int32_t rc = CHECK_DESTRUCTION();

    if (SUCCEED(rc)) {
        RWLock::AutoWLock l(mListenerLock);
        auto iter = mListener.begin();
        while (iter != mListener.end()) {
            if (iter->listener == listener) {
                iter = mListener.erase(iter);
            } else {
                iter++;
            }
        }
    }

    return rc;
}

template <typename T, typename Trait>
typename AlgorithmBase<T, Trait>::Listener *
    AlgorithmBase<T, Trait>::traverseListenerList(
    typename AlgorithmBase<T, Trait>::Listener *cur)
{
    Listener *next = NULL;

    RWLock::AutoRLock l(mListenerLock);
    if (ISNULL(cur)) {
        if (mListener.size() > 0) {
            next = &(*mListener.begin());
        }
    } else {
        for (auto iter = mListener.begin();
            iter != mListener.end(); iter++) {
            if (cur->listener == iter->listener) {
                ++iter;
                if (iter != mListener.end()) {
                    next = &(*iter);
                }
                break;
            }
        }
    }

    return next;
}

template <typename T, typename Trait>
int32_t AlgorithmBase<T, Trait>::notifyListener()
{
    int32_t rc = NO_ERROR;
    Listener *curListner = NULL;

    while (!ISNULL(curListner = traverseListenerList(curListner))) {
        if (!ISNULL(curListner->listener)) {
            curListner->listener->onAlgResultAvailable(mTask.task.taskid, &mResult);
        }
    }

    return rc;
}

template <typename T, typename Trait>
const char * const AlgorithmBase<T, Trait>::AlgEvtInf::kEvtTypeString[] = {
    [AlgorithmBase<T, Trait>::ALG_EVT_NEW_TASK]     = "new task",
    [AlgorithmBase<T, Trait>::ALG_EVT_UPDATE]       = "update result",
    [AlgorithmBase<T, Trait>::ALG_EVT_SET_PARM]     = "set parameter",
    [AlgorithmBase<T, Trait>::ALG_EVT_QUERY_STATUS] = "query status",
    [AlgorithmBase<T, Trait>::ALG_EVT_INVALID_MAX]  = "invalid event",
};

template <typename T, typename Trait>
void AlgorithmBase<T, Trait>::AlgEvtInf::clear()
{
    id   = 0;
    type = ALG_EVT_INVALID_MAX;
    buffered = false;
    job    = NULL;
}

template <typename T, typename Trait>
const char *AlgorithmBase<T, Trait>::AlgEvtInf::whoamI()
{
    return kEvtTypeString[(type < 0 || type > ALG_EVT_INVALID_MAX) ?
        ALG_EVT_INVALID_MAX : type];
}

template <typename T, typename Trait>
int32_t AlgorithmBase<T, Trait>::processTask(AlgEvtInf *evt)
{
    int32_t rc = NO_ERROR;

    switch (evt->type) {
        case ALG_EVT_NEW_TASK: {
            rc = process(static_cast<AlgNewTask *>(evt->job));
        } break;
        case ALG_EVT_SET_PARM: {
            rc = config(static_cast<AlgNewParm *>(evt->job));
        } break;
        case ALG_EVT_UPDATE: {
            rc = update(static_cast<AlgNewUpdate *>(evt->job));
        } break;
        case ALG_EVT_QUERY_STATUS: {
             rc = queryStatus(static_cast<StatusType *>(evt->job));
        } break;
        default: {
            LOGE(mModule, "Invalid evt type %s", evt->whoamI());
            rc = UNKNOWN_ERROR;
        } break;
    }

    return rc;
}

template <typename T, typename Trait>
int32_t AlgorithmBase<T, Trait>::taskDone(AlgEvtInf *evt, int32_t _rc)
{
    int32_t rc = NO_ERROR;
    typename MyType::AlgEvt type = evt->type;

    if (!SUCCEED(_rc)) {
        rc = _rc;
        LOGE(mModule, "%s process failed %d", evt->whoamI(), _rc);
    }

    if (type == ALG_EVT_NEW_TASK) {
        int32_t notifyRC = NO_ERROR;

        notifyRC = notifyListener();
        if (!SUCCEED(notifyRC)) {
            LOGE(mModule, "Failed to notify listeners");
        }

        AlgTraits<Global>::UpdateType update;
        update.type   = mAlgType;
        update.taskid = mTask.task.taskid;
        update.result = &mResult;
        update.dir    = ALG_BROADCAST_DIR_BOTH;
        notifyRC = broadcastUpdate(update);
        if (!SUCCEED(notifyRC)) {
            LOGE(mModule, "Failed to broadcast update events");
        }

        utils_atomic_dec(&mUnFishedTaskNum);
    }

    return rc;
}

template <typename T, typename Trait>
int32_t AlgorithmBase<T, Trait>::setLinker(IdType parent,
    sp<IAlgorithm> prev, sp<IAlgorithm> next)
{
    int32_t rc = CHECK_DESTRUCTION();
    bool found = false;
    RWLock::AutoWLock l(mLinkerLock);

    if (SUCCEED(rc)) {
        for (auto iter = mLinker.begin();
            iter != mLinker.end(); iter++) {
            if (iter->parent == parent) {
                iter->prev = prev;
                iter->next = next;
                iter->prevName = std::string(
                    ISNULL(prev) ? "NULL" : prev->getName());
                iter->nextName = std::string(
                    ISNULL(next) ? "NULL" : next->getName());
                if (ISNULL(prev) && ISNULL(next)) {
                    mLinker.erase(iter);
                    LOGD(mModule, "Algorithm %s linked with prev NULL "
                        "next NULL with parent id %d", whoamI(), iter->parent);
                }
                found = true;
            }
        }

        if (!found && (!ISNULL(prev) || !ISNULL(next))) {
            Linker link;
            link.parent = parent;
            link.prev = prev;
            link.next = next;
            link.prevName = std::string(
                ISNULL(prev) ? "NULL" : prev->getName());
            link.nextName = std::string(
                ISNULL(next) ? "NULL" : next->getName());
            mLinker.push_back(link);
        }

        for (auto &item : mLinker) {
            LOGD(mModule, "Algorithm %s linked with prev %s next %s "
                "with parent id %d", whoamI(),
                ISNULL(item.prev) ? "NULL" : item.prevName.c_str(),
                ISNULL(item.next) ? "NULL" : item.nextName.c_str(),
                item.parent);
        }
    }

    return NO_ERROR;
}

template <typename T, typename Trait>
typename AlgorithmBase<T, Trait>::Linker *
    AlgorithmBase<T, Trait>::traverseLinkerList(
    typename AlgorithmBase<T, Trait>::Linker *cur)
{
    Linker *next = NULL;

    RWLock::AutoRLock l(mLinkerLock);
    if (ISNULL(cur)) {
        if (mLinker.size() > 0) {
            next = &(*mLinker.begin());
        }
    } else {
        for (auto iter = mLinker.begin();
            iter != mLinker.end(); iter++) {
            if (cur->parent == iter->parent &&
                cur->next == iter->next &&
                cur->prev == iter->prev) {
                ++iter;
                if (iter != mLinker.end()) {
                    next = &(*iter);
                }
                break;
            }
        }
    }

    return next;
}

template <typename T, typename Trait>
int32_t AlgorithmBase<T, Trait>::broadcastUpdate(
    AlgTraits<Global>::UpdateType update)
{
    int32_t rc = NO_ERROR;
    Linker *curLinker = NULL;
    AlgBroadcastDir direction = update.dir;

    while (!ISNULL(curLinker = traverseLinkerList(curLinker))) {
        if (!ISNULL(curLinker->prev) &&
            (direction == ALG_BROADCAST_DIR_UPSTREAM ||
                direction == ALG_BROADCAST_DIR_BOTH)) {
            update.dir = ALG_BROADCAST_DIR_UPSTREAM;
            sp<IAlgorithm> prev = curLinker->prev.promote();
            if (!ISNULL(prev)) {
                prev->update(update);
            }
        }

        if (!ISNULL(curLinker->next) &&
            (direction == ALG_BROADCAST_DIR_DOWNSTREAM ||
                direction == ALG_BROADCAST_DIR_BOTH)) {
            update.dir = ALG_BROADCAST_DIR_DOWNSTREAM;
            sp<IAlgorithm> next = curLinker->next.promote();
            if (!ISNULL(next)) {
                next->update(update);
            }
        }
    }

    return rc;
}

template <typename T, typename Trait>
const typename AlgorithmBase<T, Trait>::queueToThreadF
    AlgorithmBase<T, Trait>::kQueuedThreadFunc[][SYNC_MAX_INVALID] = {
    {
        &AlgorithmBase<T, Trait>::queueToThread<
            AlgorithmBase<T, Trait>::ALG_EVT_NEW_TASK, SYNC_TYPE>,
        &AlgorithmBase<T, Trait>::queueToThread<
            AlgorithmBase<T, Trait>::ALG_EVT_NEW_TASK, ASYNC_TYPE>,
    },
    {
        &AlgorithmBase<T, Trait>::queueToThread<
            AlgorithmBase<T, Trait>::ALG_EVT_UPDATE, SYNC_TYPE>,
        &AlgorithmBase<T, Trait>::queueToThread<
            AlgorithmBase<T, Trait>::ALG_EVT_UPDATE, ASYNC_TYPE>,
    },
    {
        &AlgorithmBase<T, Trait>::queueToThread<
            AlgorithmBase<T, Trait>::ALG_EVT_SET_PARM, SYNC_TYPE>,
        &AlgorithmBase<T, Trait>::queueToThread<
            AlgorithmBase<T, Trait>::ALG_EVT_SET_PARM, ASYNC_TYPE>,
    },
    {
        &AlgorithmBase<T, Trait>::queueToThread<
            AlgorithmBase<T, Trait>::ALG_EVT_QUERY_STATUS, SYNC_TYPE>,
        &AlgorithmBase<T, Trait>::queueToThread<
            AlgorithmBase<T, Trait>::ALG_EVT_QUERY_STATUS, ASYNC_TYPE>,
    },
};

template <typename T, typename Trait>
template <
    typename AlgorithmBase<T, Trait>::AlgEvt type,
    SyncType::value_type sync
>
int32_t AlgorithmBase<T, Trait>::queueToThread(void *data)
{
    int32_t rc = CHECK_DESTRUCTION();
    AlgEvtInf *evt = NULL;

    if (SUCCEED(rc)) {
        bool buffered = true;
        evt = GET_OBJ_FROM_BUFFERP(mEvtObj);
        if (ISNULL(evt)) {
            evt = new AlgEvtInf();
            buffered = false;
        }
        if (ISNULL(evt)) {
            LOGE(mModule, "Failed to alloc buffered evt or new");
            rc = NO_MEMORY;
        } else {
            evt->clear();
            evt->id   = mEvtNum++;
            evt->type = type;
            evt->buffered = buffered;
            evt->job  = data;
        }
    }

    if (SUCCEED(rc)) {
        rc = mThread.run(
            [this](AlgEvtInf *_task) -> int32_t {
                int32_t _rc = processTask(_task);
                taskDone(_task, _rc);
                return _rc;
            },
            evt,
            sync);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to %s to algorithm %s thread %s",
                evt->whoamI(), whoamI(), mThread.whoamI());
        }
    }

    if (SUCCEED(rc) || !SUCCEED(rc)) {
        if (evt->buffered) {
            rc = PUT_OBJ_TO_BUFFERP(mEvtObj, evt);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to put evt to buffer, %d", rc);
            }
        } else {
            SECURE_DELETE(evt);
        }
    }

    return rc;
}

template <typename T, typename Trait>
int32_t AlgorithmBase<T, Trait>::process(TaskType &in,
    uint32_t taskId, SyncTypeE sync, std::function<int32_t (TaskType &in)> cb)
{
    int32_t rc = CHECK_DESTRUCTION();
    AlgNewTask *task = NULL;

    utils_atomic_inc(&mUnFishedTaskNum);

    if (SUCCEED(rc)) {
        bool buffered = true;
        task = GET_OBJ_FROM_BUFFERP(mTaskObj);
        if (ISNULL(task)) {
            task = (AlgNewTask *)Malloc(sizeof(AlgNewTask));
            buffered = false;
        }
        if (ISNULL(task)) {
            LOGE(mModule, "Failed to alloc buffered task or new");
            rc = NO_MEMORY;
        } else {
            task->result = NULL;
            task->taskId = taskId ? taskId : task->taskId;
            task->sync   = sync;
            task->buffered = buffered;
            task->cb     = cb;
        }
    }

    if (SUCCEED(rc)) {
        TaskTypeT *info = static_cast<TaskTypeT *>(&in);
        if (NOTNULL(info)) {
            task->info = *info;
        } else {
            ASSERT_LOG(mModule, NOTNULL(task),
                "Task should NOT be NULL, check dynamic_cast<>");
            rc = PARAM_INVALID;
        }
    }

    if (SUCCEED(rc)) {
        rc = (this->*(kQueuedThreadFunc[ALG_EVT_NEW_TASK][sync]))(task);
        if (!SUCCEED(rc)) {
            LOGD(mModule, "Failed to process on thread func, %d", rc);
        }
    }

    return rc;
}

template <typename T, typename Trait>
int32_t AlgorithmBase<T, Trait>::process(TaskType &in, TaskType &out,
    uint32_t taskId, SyncTypeE sync, std::function<int32_t (TaskType &out)> cb)
{
    int32_t rc = CHECK_DESTRUCTION();
    TaskTypeT  *tmpTask = NULL;
    AlgNewTask *task    = NULL;

    utils_atomic_inc(&mUnFishedTaskNum);

    if (SUCCEED(rc)) {
        if (sync == ASYNC_TYPE) {
            tmpTask = (TaskTypeT *)Malloc(sizeof(TaskTypeT));
            if (ISNULL(tmpTask)) {
                LOGE(mModule, "Failed to malloc %d bytes for tmp task",
                    sizeof(TaskType));
                rc = NO_MEMORY;
            }
        }
    }

    if (SUCCEED(rc)) {
        bool buffered = true;
        task = GET_OBJ_FROM_BUFFERP(mTaskObj);
        if (ISNULL(task)) {
            task = (AlgNewTask *)Malloc(sizeof(AlgNewTask));
            buffered = false;
        }
        if (ISNULL(task)) {
            LOGE(mModule, "Failed to alloc buffered task or new");
            rc = NO_MEMORY;
        } else {
            task->taskId   = taskId ? taskId : task->taskId;
            task->sync     = sync;
            task->buffered = buffered;
            task->cb       = cb;
        }
    }

    if (SUCCEED(rc)) {
        TaskTypeT *taskIn = static_cast<TaskTypeT *>(&in);
        if (NOTNULL(taskIn)) {
            task->info = *taskIn;
        } else {
            ASSERT_LOG(mModule, NOTNULL(taskIn),
                "Task should NOT be NULL, check dynamic_cast<>");
            rc = PARAM_INVALID;
        }
        TaskTypeT *taskOut = static_cast<TaskTypeT *>(&out);
        if (NOTNULL(taskOut)) {
            if (sync == ASYNC_TYPE) {
                *tmpTask = *taskOut;
                task->result = tmpTask;
            } else if (sync == SYNC_TYPE) {
                task->result = taskOut;
            }
        } else {
            ASSERT_LOG(mModule, NOTNULL(taskOut),
                "Task should NOT be NULL, check dynamic_cast<>");
            rc = PARAM_INVALID;
        }
    }

    if (SUCCEED(rc)) {
        rc = (this->*(kQueuedThreadFunc[ALG_EVT_NEW_TASK][sync]))(task);
        if (!SUCCEED(rc)) {
            LOGD(mModule, "Failed to process on thread func, %d", rc);
        }
    }

    return rc;
}

template <typename T, typename Trait>
int32_t AlgorithmBase<T, Trait>::process(AlgNewTask *task)
{
    int32_t rc = NO_ERROR;
    int32_t tmprc = NO_ERROR;

    mTask.id = mTaskNum++;

    if (SUCCEED(rc)) {
        if (mFrameDrop &&
            (mUnFishedTaskNum > static_cast<int32_t>(mMaxQueueSize))) {
            LOGI(mModule, "FRAME DROP id %d in %s unfinished task %d/%d",
                task->taskId, whoamI(), mUnFishedTaskNum, mMaxQueueSize);
            LOGD(mModule, "Use last %s result instread", whoamI());
            rc = ALREADY_EXISTS;
        }
        if (!mConstructed) {
            LOGD(mModule, "Drop frame id %d to speed up algorithm %s "
                "destruction", task->taskId, whoamI());
            rc = NOT_INITED;
        }
    }

    if (SUCCEED(rc)) {
        if (ISNULL(task->result)) {
            rc = processNewTask(task->info);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to process new task in %s", whoamI());
            }
            tmprc = task->cb(task->info);
            if (!SUCCEED(tmprc)) {
                LOGE(mModule, "Failed to call task cb, %d", rc);
            }
        }
    }

    if (SUCCEED(rc)) {
        if (NOTNULL(task->result)) {
            rc = processNewTask(task->info, *task->result);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to ProcessEx new task in %s", whoamI());
            }
            tmprc = task->cb(*task->result);
            if (!SUCCEED(tmprc)) {
                LOGE(mModule, "Failed to call task cb, %d", rc);
            }
            if (task->sync == ASYNC_TYPE) {
                SECURE_FREE(task->result);
            }
        }
    }

    if (SUCCEED(rc) || !SUCCEED(rc)) {
        if (task->buffered) {
            rc = PUT_OBJ_TO_BUFFERP(mTaskObj, task);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to put task to buffer, %d", rc);
            }
        } else {
            SECURE_FREE(task);
        }
    }

    return rc;
}

template <typename T, typename Trait>
bool AlgorithmBase<T, Trait>::busy() const
{
    return mUnFishedTaskNum;
}

template <typename T, typename Trait>
int32_t AlgorithmBase<T, Trait>::update(UpdateType &update, SyncTypeE sync)
{
    int32_t rc = CHECK_DESTRUCTION();
    UpdateTypeT *task = NULL;
    AlgNewUpdate *bufTask = NULL;

    if (SUCCEED(rc)) {
        task = static_cast<UpdateTypeT *>(&update);
        if (ISNULL(task)) {
            ASSERT_LOG(mModule, NOTNULL(task),
                "Update should NOT be NULL, check dynamic_cast<>");
            rc = PARAM_INVALID;
        }
    }

    if (SUCCEED(rc)) {
        bool buffered = true;
        bufTask = GET_OBJ_FROM_BUFFERP(mUpdateObj);
        if (ISNULL(bufTask)) {
            bufTask = (AlgNewUpdate *)Malloc(sizeof(AlgNewUpdate));
            buffered = false;
        }
        if (ISNULL(bufTask)) {
            LOGE(mModule, "Failed to alloc buffered update or new");
            rc = NO_MEMORY;
        } else {
            bufTask->info     = *task;
            bufTask->buffered = buffered;
        }
    }

    if (SUCCEED(rc)) {
        rc = (this->*(kQueuedThreadFunc[ALG_EVT_UPDATE][sync]))(bufTask);
        if (SUCCEED(rc)) {
            mUpdateNum++;
        }
    }

    if (SUCCEED(rc)) {
        rc = broadcastUpdate(update);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to broadcast algorithm update");
        }
    }

    return rc;
}

template <typename T, typename Trait>
int32_t AlgorithmBase<T, Trait>::update(AlgNewUpdate *update)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        rc = processUpdate(update->info);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Update algorithm failed on %s", whoamI());
        }
    }

    if (SUCCEED(rc) || !SUCCEED(rc)) {
        if (update->buffered) {
            rc = PUT_OBJ_TO_BUFFERP(mUpdateObj, update);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to put update to buffer, %d", rc);
            }
        } else {
            SECURE_FREE(update);
        }
    }

    return rc;
}

template <typename T, typename Trait>
int32_t AlgorithmBase<T, Trait>::config(ParmType &parm, SyncTypeE sync)
{
    int32_t rc = CHECK_DESTRUCTION();
    ParmTypeT *task = NULL;
    AlgNewParm *bufTask = NULL;

    if (SUCCEED(rc)) {
        task = static_cast<ParmTypeT *>(&parm);
        if (ISNULL(task)) {
            ASSERT_LOG(mModule, NOTNULL(task),
                "Parm should NOT be NULL, check dynamic_cast<>");
            rc = PARAM_INVALID;
        }
    }

    if (SUCCEED(rc)) {
        bool buffered = true;
        bufTask = GET_OBJ_FROM_BUFFERP(mParmObj);
        if (ISNULL(bufTask)) {
            bufTask = (AlgNewParm *)Malloc(sizeof(AlgNewParm));
            buffered = false;
        }
        if (ISNULL(bufTask)) {
            LOGE(mModule, "Failed to alloc buffered update or new");
            rc = NO_MEMORY;
        } else {
            bufTask->info     = *task;
            bufTask->buffered = buffered;
        }
    }

    if (SUCCEED(rc)) {
        rc = (this->*(kQueuedThreadFunc[ALG_EVT_SET_PARM][sync]))(bufTask);
        if (SUCCEED(rc)) {
            mParmNum++;
        }
    }

    return rc;
}

template <typename T, typename Trait>
int32_t AlgorithmBase<T, Trait>::config(AlgNewParm *parm)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        rc = processParms(parm->info);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Set algorithm parm failed");
        }
    }

    if (SUCCEED(rc) || !SUCCEED(rc)) {
        if (parm->buffered) {
            rc = PUT_OBJ_TO_BUFFERP(mParmObj, parm);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to put parm to buffer, %d", rc);
            }
        } else {
            SECURE_FREE(parm);
        }
    }

    return rc;
}

template <typename T, typename Trait>
int32_t AlgorithmBase<T, Trait>::queryStatus(StatusType &status)
{
    int32_t rc = CHECK_DESTRUCTION();

    if (SUCCEED(rc)) {
        rc = (this->*(kQueuedThreadFunc[ALG_EVT_QUERY_STATUS][SYNC_TYPE]))(&status);
        if (SUCCEED(rc)) {
            mStatusNum++;
        } else {
            LOGE(mModule, "Failed to query status.", rc);
        }
    }

    return rc;
}

template <typename T, typename Trait>
int32_t AlgorithmBase<T, Trait>::queryStatus(StatusType *_status)
{
    int32_t rc = NO_ERROR;
    StatusTypeT *status = NULL;

    if (SUCCEED(rc)) {
        status = static_cast<StatusTypeT *>(_status);
        if (ISNULL(status)) {
            ASSERT_LOG(mModule, NOTNULL(status),
                "Status should NOT be NULL, check dynamic_cast<>");
            rc = PARAM_INVALID;
        }
    }

    if (SUCCEED(rc)) {
        rc = processQueryStatus(*status);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Query status failed.", rc);
        }
    }

    return rc;
}

template <typename T, typename Trait>
const char *AlgorithmBase<T, Trait>::getName() const
{
    return whoamI();
}

template <typename T, typename Trait>
AlgType AlgorithmBase<T, Trait>::getType() const
{
    return mAlgType;
}

template <typename T, typename Trait>
AlgCaps AlgorithmBase<T, Trait>::queryCap() const
{
    return mCaps;
}

template <typename T, typename Trait>
typename AlgorithmBase<T, Trait>::AlgorithmType *
    AlgorithmBase<T, Trait>::getAlgorithmInDanger()
{
    return getAlgorithmInDangerInternal();
}

};

#endif
