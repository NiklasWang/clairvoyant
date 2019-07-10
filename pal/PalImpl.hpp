#ifndef __PAL_IMPL_HPP_
#define __PAL_IMPL_HPP_

#include "PalImpl.h"

namespace pandora {

#define PAL_DELETE_PUT_BUFFERED(buffer, obj, buffered) \
    if (!buffered && NOTNULL(obj)) { \
        delete obj; \
    } \
    if (buffered && NOTNULL(obj)) { \
        PUT_OBJ_TO_BUFFERP(buffer, obj); \
    }

template<typename T, PalParmType t>
int32_t PalImpl::attachThread(T &result, SyncTypeE sync)
{
    int32_t rc = NO_ERROR;
    bool bufferedParm = true;
    bool bufferedTask = true;
    PalParms *parm    = NULL;
    TaskInf  *task    = NULL;

    if (SUCCEED(rc)) {
        task = GET_OBJ_FROM_BUFFERP(mTaskBuf);
        if (ISNULL(task)) {
            task = new TaskInf();
            bufferedTask = false;
        }
        if (ISNULL(task)) {
            LOGE(mModule, "Failed to alloc buffered task info or new");
            rc = NO_MEMORY;
        } else {
            task->clear();
            task->sync = sync;
            task->bufMe = bufferedTask;
        }
    }

    if (SUCCEED(rc)) {
        parm = GET_OBJ_FROM_BUFFERP(mParmsBuf);
        if (ISNULL(parm)) {
            parm = new PalParms();
            bufferedParm = false;
        }
        if (ISNULL(parm)) {
            LOGE(mModule, "Failed to alloc buffered pal parm or new");
            rc = NO_MEMORY;
        } else {
            *parm<<result;
            task->set(parm, bufferedParm);
        }
    }

    if (SUCCEED(rc)) {
        rc = mThreads->run<TaskInf>(
            [this](TaskInf *_task) -> int32_t {
                int32_t _rc = processTask(_task);
                taskDone(_task, _rc);
                if (_task->sync == ASYNC_TYPE) {
                    PAL_DELETE_PUT_BUFFERED(mParmsBuf,
                        _task->getpp(), _task->bufParm);
                }
                return _rc;
            },
            task,
            sync
        );
        if (!SUCCEED(rc)) {
            LOGD(mModule, "Failed to run task on thread, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        if (sync == SYNC_TYPE) {
            rc = task->rc;
            if (!SUCCEED(rc)) {
                LOGD(mModule, "Failed to do %s", parm->getName());
            }
        }
    }

    if (SUCCEED(rc)) {
        if (sync == SYNC_TYPE) {
            if (t == PARM_TYPE_GET || t == PARM_TYPE_CONFIG) {
                *parm>>result;
            }
        }
    }

    if (SUCCEED(rc) || !SUCCEED(rc)) {
        if (sync == SYNC_TYPE) {
            PAL_DELETE_PUT_BUFFERED(mParmsBuf, parm, bufferedParm);
        }
        if (sync == SYNC_TYPE || sync == ASYNC_TYPE) {
            PAL_DELETE_PUT_BUFFERED(mTaskBuf,  task, bufferedTask);
        }
    }

    return rc;
}

template<typename T>
int32_t PalImpl::getParm(T &result, SyncTypeE sync)
{
    return attachThread<T, PARM_TYPE_GET>(result, sync);
}

template<typename T>
int32_t PalImpl::setParm(T &parm, SyncTypeE sync)
{
    return attachThread<T, PARM_TYPE_SET>(parm, sync);
}

template<typename T>
int32_t PalImpl::getConfig(T &config, SyncTypeE sync)
{
    return attachThread<T, PARM_TYPE_CONFIG>(config, sync);
}

template<typename T>
int32_t PalImpl::evtNotify(T &evt, SyncTypeE sync)
{
    return attachThread<T, PARM_TYPE_OTHERS>(evt, sync);
}

template<typename T>
int32_t PalImpl::dataNotify(T &data, SyncTypeE sync)
{
    return attachThread<T, PARM_TYPE_OTHERS>(data, sync);
}

};

#endif
