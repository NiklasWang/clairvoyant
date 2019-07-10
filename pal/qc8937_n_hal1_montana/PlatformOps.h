#ifndef _PLATFORM_FOR_QC8996_N_HAL1_H_
#define _PLATFORM_FOR_QC8996_N_HAL1_H_

#include "PlatformOpsIntf.h"

namespace qcamera {
    class QCamera2HardwareInterface;
};

namespace pandora {

class PlatformOps :
    public PlatformOpsIntf {
public:
    explicit PlatformOps(qcamera::QCamera2HardwareInterface *hwi);

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
    bool       mIsZuiCamera;
    ModuleType mModule;
    PlatformPrivSet mLastSharpenDenoiseParm;
    qcamera::QCamera2HardwareInterface *mHWI;
};

};

#endif
