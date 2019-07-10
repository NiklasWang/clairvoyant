#include "PandoraImpl.h"

namespace pandora {

PandoraImpl::PandoraImpl(
    PlatformOpsIntf *platform) :
    mConstructed(false),
    mModule(MODULE_PANDORA_IMPL),
    mTaskCnt(0),
    mPlatform(platform)
{
    LOGI(mModule, "%s %s initializing...", PROJNAME, VERSION);
}

PandoraImpl::~PandoraImpl()
{
    if (mConstructed) {
        destruct();
    }

    LOGD(mModule, "Pandora impl destructed");
}

int32_t PandoraImpl::construct()
{
    int32_t rc = NO_ERROR;

    if (mConstructed) {
        rc = ALREADY_EXISTS;
    }

    if (SUCCEED(rc)) {
        mCore = new PandoraCore(mPlatform);
        if (ISNULL(mCore)) {
            LOGE(mModule, "Failed to create core");
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        rc = mCore->construct();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to construct core");
        }
    }

    if (SUCCEED(rc)) {
        mThreads = ThreadPoolEx::getInstance();
        if (ISNULL(mThreads)) {
            LOGE(mModule, "Failed to get thread pool");
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        mConstructed = true;
        LOGD(mModule, "pandora impl constructed");
    }

    return RETURNIGNORE(rc, ALREADY_EXISTS);
}

int32_t PandoraImpl::destruct()
{
    int32_t rc = NO_ERROR;

    if (!mConstructed) {
        rc = NOT_INITED;
    } else {
        mConstructed = false;
    }

    if (SUCCEED(rc)) {
        rc = mCore->destruct();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to destruct core");
        } else {
            SECURE_DELETE(mCore);
        }
    }

    if (SUCCEED(rc)) {
        mThreads->removeInstance();
    }

    if (!SUCCEED(rc)) {
        mConstructed = true;
        LOGE(mModule, "Failed to destruct pandora impl");
    } else {
        LOGD(mModule, "pandora impl destructed");
    }

    return RETURNIGNORE(rc, NOT_INITED);

}

const char * const PandoraImpl::ThreadTaskBase::kTaskTypeString[] = {
    [TT_PREPARE_PREVIEW]     = "prepare preview",
    [TT_UNPREPARE_PREVIEW]   = "unprepare preview",
    [TT_PREPARE_SNAPSHOT]    = "prepare snapshot",
    [TT_UNPREPARE_SNAPSHOT]  = "unprepare snapshot",
    [TT_PREPARE_RECORDING]   = "prepare recording",
    [TT_UNPREPARE_RECORDING] = "unprepare recording",
    [TT_METADATA_AVAILABLE]  = "metadata available",
    [TT_COMMAND_AVAILABLE]   = "command available",
    [TT_PARAMETER_AVAILABLE] = "parameter available",
    [TT_FRAME_READY]         = "frame ready",
    [TT_MAX_INVALID]         = "invalid task type",
};

const char *PandoraImpl::ThreadTaskBase::whoamI()
{
    return kTaskTypeString[
        (taskType < 0 || taskType > TT_MAX_INVALID) ?
        TT_MAX_INVALID : taskType];
}

int32_t PandoraImpl::processTask(ThreadTaskBase *dat)
{
    assert(!ISNULL(dat));
    uint32_t rc = NO_ERROR;
    ThreadTaskType type = dat->getTaskType();

    switch (type) {
        case TT_PREPARE_PREVIEW: {
            ThreadTask<TT_PREPARE_PREVIEW> *evt =
                static_cast<ThreadTask<TT_PREPARE_PREVIEW> *>(dat);
            rc = evt->bottomrc = startPreview(evt->task);
        } break;
        case TT_UNPREPARE_PREVIEW: {
            ThreadTask<TT_UNPREPARE_PREVIEW> *evt =
                static_cast<ThreadTask<TT_UNPREPARE_PREVIEW> *>(dat);
            rc = evt->bottomrc = stopPreview(evt->task);
        } break;
        case TT_PREPARE_SNAPSHOT: {
            ThreadTask<TT_PREPARE_SNAPSHOT> *evt =
                static_cast<ThreadTask<TT_PREPARE_SNAPSHOT> *>(dat);
            rc = evt->bottomrc = takePicture(evt->task);
        } break;
        case TT_UNPREPARE_SNAPSHOT: {
            ThreadTask<TT_UNPREPARE_SNAPSHOT> *evt =
                static_cast<ThreadTask<TT_UNPREPARE_SNAPSHOT> *>(dat);
            rc = evt->bottomrc = pictureTaken(evt->task);
        } break;
        case TT_PREPARE_RECORDING: {
            ThreadTask<TT_PREPARE_RECORDING> *evt =
                static_cast<ThreadTask<TT_PREPARE_RECORDING> *>(dat);
            rc = evt->bottomrc = startRecording(evt->task);
        } break;
        case TT_UNPREPARE_RECORDING: {
            ThreadTask<TT_UNPREPARE_RECORDING> *evt =
                static_cast<ThreadTask<TT_UNPREPARE_RECORDING> *>(dat);
            rc = evt->bottomrc = stopRecording(evt->task);
        } break;
        case TT_METADATA_AVAILABLE: {
            ThreadTask<TT_METADATA_AVAILABLE> *evt =
                static_cast<ThreadTask<TT_METADATA_AVAILABLE> *>(dat);
            rc = evt->bottomrc = onMetadataAvailable(evt->task);
        } break;
        case TT_COMMAND_AVAILABLE: {
            ThreadTask<TT_COMMAND_AVAILABLE> *evt =
                static_cast<ThreadTask<TT_COMMAND_AVAILABLE> *>(dat);
            rc = evt->bottomrc = onCommandAvailable(evt->task);
        } break;
        case TT_PARAMETER_AVAILABLE: {
            ThreadTask<TT_PARAMETER_AVAILABLE> *evt =
                static_cast<ThreadTask<TT_PARAMETER_AVAILABLE> *>(dat);
            rc = evt->bottomrc = onParameterAvailable(evt->task);
        } break;
        case TT_FRAME_READY: {
            ThreadTask<TT_FRAME_READY> *evt =
                static_cast<ThreadTask<TT_FRAME_READY> *>(dat);
            rc = evt->bottomrc = onFrameReady(evt->task);
        } break;
        case TT_MAX_INVALID:
        default: {
            LOGE(mModule, "Invalid task type %s", dat->whoamI());
            rc = UNKNOWN_ERROR;
        } break;
    }

    return rc;
}

int32_t PandoraImpl::taskDone(ThreadTaskBase *evt, int32_t processRC)
{
    int32_t rc = NO_ERROR;

    if (!SUCCEED(processRC)) {
        LOGD(mModule, "Failed to process evt %s with %d, trying to ignore",
            evt->whoamI(), processRC);
    }

    return rc;
}

template <ThreadTaskType type, SyncTypeE sync>
int32_t PandoraImpl::addTaskToThread(void *value)
{
    int32_t rc = NO_ERROR;
    ThreadTask<type> *evt = NULL;
    ReqArgs<type> *reqArgs = static_cast<ReqArgs<type> *>(value);

    evt = new ThreadTask<type>(sync);
    if (ISNULL(evt)) {
        LOGE(mModule, "Failed to alloc memory");
        rc = NO_MEMORY;
    } else {
        evt->id = mTaskCnt++;
        evt->bottomrc = NO_ERROR;
        if (NOTNULL(value)) {
            evt->task = *reqArgs;
        }
    }

    if (SUCCEED(rc)) {
        rc = mThreads->run<ThreadTaskBase>(
            [this](ThreadTaskBase *_task) -> int32_t {
                int32_t _rc = processTask(_task);
                taskDone(_task, _rc);
                return _rc;
            },
            evt,
            sync
        );
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to push evt %d %s to thread pool, %d",
                evt->id, evt->whoamI(), rc);
        }
    }

    if (SUCCEED(rc)) {
        if (evt->sync == SYNC_TYPE) {
            rc = evt->bottomrc;
        }
        if (evt->sync == SYNC_TYPE || evt->sync == ASYNC_TYPE) {
            SECURE_DELETE(evt);
        }
    }

    return rc;
}

const PandoraImpl::TAddTaskToThread
    PandoraImpl::kAddThreadTaskFunc[] = {
    &PandoraImpl::addTaskToThread<TT_PREPARE_PREVIEW,     SYNC_TYPE>,
    &PandoraImpl::addTaskToThread<TT_UNPREPARE_PREVIEW,   SYNC_TYPE>,
    &PandoraImpl::addTaskToThread<TT_PREPARE_SNAPSHOT,    SYNC_TYPE>,
    &PandoraImpl::addTaskToThread<TT_UNPREPARE_SNAPSHOT,  SYNC_TYPE>,
    &PandoraImpl::addTaskToThread<TT_PREPARE_RECORDING,   SYNC_TYPE>,
    &PandoraImpl::addTaskToThread<TT_UNPREPARE_RECORDING, SYNC_TYPE>,
    &PandoraImpl::addTaskToThread<TT_METADATA_AVAILABLE,  SYNC_TYPE>,
    &PandoraImpl::addTaskToThread<TT_COMMAND_AVAILABLE,   SYNC_TYPE>,
    &PandoraImpl::addTaskToThread<TT_PARAMETER_AVAILABLE, SYNC_TYPE>,
    &PandoraImpl::addTaskToThread<TT_FRAME_READY,         SYNC_TYPE>,
};

#define CONSTRUCT_IMPL() \
    do { \
        int32_t rc = NO_ERROR; \
        if (!mConstructed) { \
            rc = construct(); \
            if (!SUCCEED(rc)) { \
                LOGE(mModule, "Failed to construct pandora impl %d", rc); \
                return rc; \
            } \
        } \
    } while(0)

int32_t PandoraImpl::startPreview()
{
    CONSTRUCT_IMPL();
    return (this->*(kAddThreadTaskFunc[TT_PREPARE_PREVIEW]))(NULL);
}

int32_t PandoraImpl::stopPreview()
{
    CONSTRUCT_IMPL();
    return (this->*(kAddThreadTaskFunc[TT_UNPREPARE_PREVIEW]))(NULL);
}

int32_t PandoraImpl::takePicture()
{
    CONSTRUCT_IMPL();
    return (this->*(kAddThreadTaskFunc[TT_PREPARE_SNAPSHOT]))(NULL);
}

int32_t PandoraImpl::pictureTaken()
{
    CONSTRUCT_IMPL();
    return (this->*(kAddThreadTaskFunc[TT_UNPREPARE_SNAPSHOT]))(NULL);
}

int32_t PandoraImpl::startRecording()
{
    CONSTRUCT_IMPL();
    return (this->*(kAddThreadTaskFunc[TT_PREPARE_RECORDING]))(NULL);
}

int32_t PandoraImpl::stopRecording()
{
    CONSTRUCT_IMPL();
    return (this->*(kAddThreadTaskFunc[TT_UNPREPARE_RECORDING]))(NULL);
}

int32_t PandoraImpl::onMetadataAvailable(void *metadata)
{
    CONSTRUCT_IMPL();
    ReqArgs<TT_METADATA_AVAILABLE> task = { .ptr = metadata };
    return (this->*(kAddThreadTaskFunc[TT_METADATA_AVAILABLE]))(&task);
}

int32_t PandoraImpl::onCommandAvailable(
    int32_t command, int32_t arg1, int32_t arg2)
{
    CONSTRUCT_IMPL();
    ReqArgs<TT_COMMAND_AVAILABLE> task = {
        .command = command,
        .arg1 = arg1,
        .arg2 = arg2,
    };
    return (this->*(kAddThreadTaskFunc[TT_COMMAND_AVAILABLE]))(&task);
}

int32_t PandoraImpl::onParameterAvailable(const void *parameter)
{
    CONSTRUCT_IMPL();
    ReqArgs<TT_PARAMETER_AVAILABLE> task = { .parm = parameter };
    return (this->*(kAddThreadTaskFunc[TT_PARAMETER_AVAILABLE]))(&task);
}

int32_t PandoraImpl::onFrameReady(FrameInfo &frame)
{
    CONSTRUCT_IMPL();
    ReqArgs<TT_FRAME_READY> task = {
        .frame = frame,
    };
    return (this->*(kAddThreadTaskFunc[TT_FRAME_READY]))(&task);
}

int32_t PandoraImpl::startPreview(
    ReqArgs<TT_PREPARE_PREVIEW> &/*task*/)
{
    return ISNULL(mCore) ?
        NOT_INITED : mCore->startPreview();
}

int32_t PandoraImpl::stopPreview(
    ReqArgs<TT_UNPREPARE_PREVIEW> &/*task*/)
{
    return ISNULL(mCore) ?
        NOT_INITED : mCore->stopPreview();
}

int32_t PandoraImpl::takePicture(
    ReqArgs<TT_PREPARE_SNAPSHOT> &/*task*/)
{
    return ISNULL(mCore) ?
        NOT_INITED : mCore->takePicture();
}

int32_t PandoraImpl::pictureTaken(
    ReqArgs<TT_UNPREPARE_SNAPSHOT> &/*task*/)
{
    return ISNULL(mCore) ?
         NOT_INITED : mCore->finishPicture();
}

int32_t PandoraImpl::startRecording(
    ReqArgs<TT_PREPARE_RECORDING> &/*task*/)
{
    return ISNULL(mCore) ?
        NOT_INITED : mCore->startRecording();
}

int32_t PandoraImpl::stopRecording(
    ReqArgs<TT_UNPREPARE_RECORDING> &/*task*/)
{
    return ISNULL(mCore) ?
         NOT_INITED : mCore->stopRecording();
}

int32_t PandoraImpl::onMetadataAvailable(
    ReqArgs<TT_METADATA_AVAILABLE> &task)
{
    return ISNULL(mCore) ?
        NOT_INITED : mCore->updateMetadata(task);
}

int32_t PandoraImpl::onCommandAvailable(
    ReqArgs<TT_COMMAND_AVAILABLE> &task)
{
    return ISNULL(mCore) ?
        NOT_INITED : mCore->updateCommand(task);
}

int32_t PandoraImpl::onParameterAvailable(
    ReqArgs<TT_PARAMETER_AVAILABLE> &task)
{
    return ISNULL(mCore) ?
        NOT_INITED : mCore->updateParameter(task);
}

int32_t PandoraImpl::onFrameReady(
    ReqArgs<TT_FRAME_READY> &task)
{
    return ISNULL(mCore) ?
        NOT_INITED : mCore->onframeReady(task);
}

};

