#ifndef __PAL_IMPL_H_
#define __PAL_IMPL_H_

#include "Common.h"
#include "ThreadPoolEx.h"
#include "SyncType.h"
#include "PalParms.h"
#include "ConfigInterface.h"
#include "ObjectBuffer.h"
#include "RefBase.h"

namespace pandora {

class PlatformOpsIntf;

class PalImpl :
    virtual public noncopyable {

public:
    PalImpl(PlatformOpsIntf *platform);
    ~PalImpl();
    int32_t construct();
    int32_t destruct();

public:
    int32_t onMetadataAvailable(const void *metadata);
    int32_t onParameterAvailable(const void *data);
    int32_t onCommandAvailable(int32_t cmd, int32_t arg1, int32_t arg2);


public:
    template<typename T>
    int32_t getParm(T &parm, SyncTypeE sync);

    template<typename T>
    int32_t setParm(T &parm, SyncTypeE sync);

    template<typename T>
    int32_t getConfig(T &parm, SyncTypeE sync);

    template<typename T>
    int32_t evtNotify(T &evt, SyncTypeE sync);

    template<typename T>
    int32_t dataNotify(T &data, SyncTypeE sync);

private:
    template<typename T, PalParmType t>
    int32_t attachThread(T &result, SyncTypeE sync);

    int32_t doAction(PalParms *parm);
    ParmCategoryType getCategory(PalParms *parm);
    int32_t waitForThread(ParmCategoryType category);
    int32_t releaseThread(ParmCategoryType category);

public:
    struct TaskInf :
        public Identifier {
    public:
        TaskInf(ModuleType module = MODULE_PAL_IMPL);
        TaskInf(const TaskInf &rhs);
        uint32_t id();
        PalParms *getpp();
        void set(PalParms *p, bool buffered);
        void clear();

    public:
        bool      bufParm;
        bool      bufMe;
        SyncTypeE sync;
        int32_t   rc;

    private:
        PalParms *parm;
        uint32_t  index;
        static uint32_t cnt;
    };

private:
    int32_t processTask(TaskInf *task);
    int32_t taskDone(TaskInf *task, int32_t rc);

private:
    template<typename T>
    struct ObjectBufferEx :
        public ObjectBuffer<T>,
        public RefBase {
    };

    struct ThreadInf :
        public RefBase {
        Semaphore sem; // Initialize with 1, do not block first runner
        ParmCategoryType category;
    public:
        ThreadInf();
    };

private:
    bool             mConstructed;
    ModuleType       mModule;
    PlatformOpsIntf *mPlatform;
    ThreadPoolEx    *mThreads;
    RWLock           mThreadInfLock;
    std::list<sp<ThreadInf> >     mThreadInf;
    sp<ObjectBufferEx<PalParms> > mParmsBuf;
    sp<ObjectBufferEx<TaskInf> >  mTaskBuf;
};

};

#include "PalImpl.hpp"

#endif
