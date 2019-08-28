#ifndef _PLATFORM_FOR_QC8953_O_HAL1_H_
#define _PLATFORM_FOR_QC8953_O_HAL1_H_

#include "PlatformOpsIntf.h"


namespace pandora {

#define SYSTEM_PROP_MAX_LEN 128
#define SENSOR_NAME_MAX_LEN 32

class PlatformOps :
    public PlatformOpsIntf {
public:
    explicit PlatformOps();

public:
    virtual bool isInited() override;
    virtual const char *whoamI() override;
    virtual sp<ConfigInterface> &getConfig() override;
    virtual int32_t onMetadataAvailable(const void *metadata) override;
    virtual int32_t onParameterAvailable(const void *data) override;
    virtual int32_t onCommandAvailable(int32_t cmd, int32_t arg1, int32_t arg2) override;

private:
    virtual int32_t setMultiShot(MultiShotParm &parm) override;
    virtual int32_t setMultiExposure(MultiExporeParm &parm) override;
    virtual int32_t setPlatformPrivSetting(PlatformPrivSet &parm) override;

    virtual int32_t sendEvtCallback(EvtCallbackInf &evt) override;
    virtual int32_t sendDataCallback(DataCallbackInf &data) override;

    void init();

private:
    ModuleType mModule;
    char       mSensorName[SENSOR_NAME_MAX_LEN];
    char       mProductName[SYSTEM_PROP_MAX_LEN];
    char       mBuildVersion[SYSTEM_PROP_MAX_LEN];
    char       mPackageName[SYSTEM_PROP_MAX_LEN];
    int32_t    mDeviceDpi;
    bool       mDebugBuild;
    PlatformPrivSet mLastPlatformPrivSet;
};

};

#endif
