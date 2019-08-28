#ifndef _PANDORA_IMPL_
#define _PANDORA_IMPL_

#include "Common.h"
#include "Pandora.h"
#include "SyncType.h"
#include "ThreadPoolEx.h"
#include "PandoraCore.h"
#include "PandoraImplHelper.h"

namespace pandora {

class PlatformOpsIntf;

class PandoraImpl :
    public PandoraInterface,
    virtual public noncopyable {

public:
    int32_t startPreview() override;
    int32_t stopPreview() override;
    int32_t takePicture() override;
    int32_t pictureTaken() override;
    int32_t startRecording()override;
    int32_t stopRecording()override;
    int32_t onMetadataAvailable(void *metadata) override;
    int32_t onCommandAvailable(int32_t command,
        int32_t arg1 = 0, int32_t arg2 = 0) override;
    int32_t onParameterAvailable(const void *parameter) override;
    int32_t onFrameReady(FrameInfo &frame) override;

private:
    int32_t construct();
    int32_t destruct();
    int32_t startPreview(ReqArgs<TT_PREPARE_PREVIEW> &task);
    int32_t stopPreview(ReqArgs<TT_UNPREPARE_PREVIEW> &task);
    int32_t takePicture(ReqArgs<TT_PREPARE_SNAPSHOT> &task);
    int32_t pictureTaken(ReqArgs<TT_UNPREPARE_SNAPSHOT> &task);
    int32_t startRecording(ReqArgs<TT_PREPARE_RECORDING> &task);
    int32_t stopRecording(ReqArgs<TT_UNPREPARE_RECORDING> &task);
    int32_t onMetadataAvailable(ReqArgs<TT_METADATA_AVAILABLE> &task);
    int32_t onCommandAvailable(ReqArgs<TT_COMMAND_AVAILABLE> &task);
    int32_t onParameterAvailable(ReqArgs<TT_PARAMETER_AVAILABLE> &task);
    int32_t onFrameReady(ReqArgs<TT_FRAME_READY> &task);

public:
    explicit PandoraImpl(PlatformOpsIntf *platform);
    virtual ~PandoraImpl();

public:
    struct ThreadTaskBase :
        public Identifier {
        uint32_t id;
        ThreadTaskType taskType;
        SyncTypeE sync;
        uint32_t bottomrc;
        static const char * const kTaskTypeString[];
     public:
        const char *whoamI();
        ThreadTaskType getTaskType() { return taskType; }
        explicit ThreadTaskBase(
            ThreadTaskType type = TT_MAX_INVALID,
            SyncTypeE _sync = SYNC_TYPE,
            ModuleType module = MODULE_PANDORA_IMPL) :
            Identifier(module),
            taskType(type), sync(_sync) {}
        ~ThreadTaskBase() = default;
    };

    template <ThreadTaskType type>
    struct ThreadTask :
        public ThreadTaskBase {
        ReqArgs<type> task;
        explicit ThreadTask(SyncTypeE _sync = SYNC_TYPE) :
            ThreadTaskBase(type, _sync) {}
        ~ThreadTask() = default;
    };

private:
    typedef int32_t (PandoraImpl::*TAddTaskToThread)(void *value);
    template <ThreadTaskType type, SyncTypeE sync = SYNC_TYPE>
    int32_t addTaskToThread(void *value);

    int32_t processTask(ThreadTaskBase *dat);
    int32_t taskDone(ThreadTaskBase *dat, int32_t rc);

private:
    bool          mConstructed;
    ModuleType    mModule;
    uint32_t      mTaskCnt;
    ThreadPoolEx *mThreads;
    PandoraCore  *mCore;
    PlatformOpsIntf *mPlatform;
    static const TAddTaskToThread kAddThreadTaskFunc[];
};

};

#endif
