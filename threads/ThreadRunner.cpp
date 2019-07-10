#include "PalImpl.h"
#include "PandoraImpl.h"
#include "Thread.h"

namespace pandora {

int32_t Thread::callFunc(TaskBase *task, bool release)
{
    int32_t rc = NO_ERROR;

    if (task->module == MODULE_THREAD_POOL) {
        TaskInf<InternalTask> *internal =
            static_cast<TaskInf<InternalTask> *>(task);
        rc = internal->func(internal->arg);
        if (release && NOTNULL(internal) && NOTNULL(internal->arg)) {
            SECURE_DELETE(internal->arg);
        }
    }

    if (task->module == MODULE_PANDORA_IMPL) {
        TaskInf<PandoraImpl::ThreadTaskBase> *pandora =
            static_cast<TaskInf<PandoraImpl::ThreadTaskBase> *>(task);
        rc = pandora->func(pandora->arg);
        if (release && NOTNULL(pandora) && NOTNULL(pandora->arg)) {
            SECURE_DELETE(pandora->arg);
        }
    }

    if (task->module == MODULE_PAL_IMPL) {
        TaskInf<PalImpl::TaskInf> *pal =
            static_cast<TaskInf<PalImpl::TaskInf> *>(task);
        rc = pal->func(pal->arg);
        if (release && NOTNULL(pal) && NOTNULL(pal->arg)) {
            SECURE_DELETE(pal->arg);
        }
    }

    return rc;
}

}
