#ifndef _PANDORA_PAL_H_
#define _PANDORA_PAL_H_

#include "SyncType.h"
#include "Modules.h"

namespace pandora {

class PlatformOpsIntf;
class PalImpl;

class Pal
{
public:
    Pal(PlatformOpsIntf *platform);
    ~Pal();

public:
    int32_t onMetadataAvailable(const void *metadata);
    int32_t onParameterAvailable(const void *data);
    int32_t onCommandAvailable(int32_t cmd, int32_t arg1, int32_t arg2);

public:
    template<typename T>
    int32_t getParm(T &parm, SyncTypeE sync = SYNC_TYPE);

    template<typename T>
    int32_t setParm(T &parm, SyncTypeE sync = SYNC_TYPE);

    template<typename T>
    int32_t getConfig(T &parm, SyncTypeE sync = SYNC_TYPE);

    template<typename T>
    int32_t evtNotify(T &evt, SyncTypeE sync = ASYNC_TYPE);

    template<typename T>
    int32_t dataNotify(T &data, SyncTypeE sync = ASYNC_TYPE);

public:
    int32_t construct();
    int32_t destruct();

private:
    bool             mConstructed;
    ModuleType       mModule;
    PlatformOpsIntf *mPlatform;
    PalImpl         *mPalImpl;
};

};

#include "Pal.hpp"

#endif
