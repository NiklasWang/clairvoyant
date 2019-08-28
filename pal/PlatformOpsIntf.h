#ifndef _PLATFORM_OPERATION_INTF__
#define _PLATFORM_OPERATION_INTF__

#include "PalParms.h"
#include "CameraStatus.h"
#include "RWLock.h"

namespace pandora {

class PlatformOpsIntf
{
public:
    virtual const char *whoamI() = 0;
    virtual sp<ConfigInterface> &getConfig() = 0;

public:
    virtual bool isInited();
    int32_t requestHandler(PalParms *request);
    virtual int32_t onMetadataAvailable(const void *metadata) = 0;
    virtual int32_t onParameterAvailable(const void *data) = 0;
    virtual int32_t onCommandAvailable(int32_t cmd, int32_t arg1, int32_t arg2) = 0;

private:
    #define DECLARE_GET_FUNC(type) \
        virtual int32_t get##type(type &res);

    DECLARE_GET_FUNC(PreviewDim);
    DECLARE_GET_FUNC(PictureDim);
    DECLARE_GET_FUNC(VideoDim);
    DECLARE_GET_FUNC(FlashMode);
    DECLARE_GET_FUNC(HdrMode);
    DECLARE_GET_FUNC(LongshotMode);
    DECLARE_GET_FUNC(LongExpMode);
    DECLARE_GET_FUNC(SmartshotMode);
    DECLARE_GET_FUNC(ImageStabMode);
    DECLARE_GET_FUNC(VideoStabMode);
    DECLARE_GET_FUNC(FlipMode);
    DECLARE_GET_FUNC(NightShotMode);
    DECLARE_GET_FUNC(BeautySetting);
    DECLARE_GET_FUNC(FaceInfo);
    DECLARE_GET_FUNC(AEInfo);
    DECLARE_GET_FUNC(AWBInfo);
    DECLARE_GET_FUNC(AFInfo);
    DECLARE_GET_FUNC(HdrInfo);
    DECLARE_GET_FUNC(SceneInfo);
    DECLARE_GET_FUNC(CameraTypeInf);
    DECLARE_GET_FUNC(CommandInf);
    DECLARE_GET_FUNC(DualCamMode);
    DECLARE_GET_FUNC(SubCamData);
    DECLARE_GET_FUNC(FairLightMode);
    DECLARE_GET_FUNC(RotationAngle);
    DECLARE_GET_FUNC(WaterMarkInfo);
    DECLARE_GET_FUNC(ZoomInf);
    DECLARE_GET_FUNC(SingleBokehInf);
    DECLARE_GET_FUNC(PlatformPriv);

    #define DECLARE_CONFIG_FUNC(func, type) \
        virtual int32_t func(type &res);
#if 0
    DECLARE_CONFIG_FUNC(getHdrConfiThres,   ConfigInterface::ThresType);
    DECLARE_CONFIG_FUNC(getExtMsgID,        ConfigInterface::IdType);
    DECLARE_CONFIG_FUNC(getFocusEndConfig,  FocusEndConfig);
    DECLARE_CONFIG_FUNC(getExpChangeConfig, ExpChangeConfig);
    DECLARE_CONFIG_FUNC(getNightStabConfig, NightStabConfig);
    DECLARE_CONFIG_FUNC(getLowLightConfig,  LowLightConfig);
    DECLARE_CONFIG_FUNC(getParmsCategory,   ParmsCategory);
#endif
    virtual int32_t setMultiShot(MultiShotParm &parm) = 0;
    virtual int32_t setMultiExposure(MultiExporeParm &parm) = 0;
    virtual int32_t setPlatformPrivSetting(PlatformPrivSet &parm) = 0;

    virtual int32_t sendEvtCallback(EvtCallbackInf &evt) = 0;
    virtual int32_t sendDataCallback(DataCallbackInf &data) = 0;

public:
    PlatformOpsIntf();
    virtual ~PlatformOpsIntf() = default;

public:
    sp<ConfigInterface> mConfig;
    bool              mParmValid[GET_PARM_MAX_INVALID];
    PalParms::_g::_u  mGetParms[GET_PARM_MAX_INVALID];
    RWLock            mParmLock;
};

};

#endif
