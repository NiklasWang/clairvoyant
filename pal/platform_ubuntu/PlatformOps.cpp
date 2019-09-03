#include "Common.h"
#include "PlatformOps.h"
#include "Platform.h"
#include "CameraStatus.h"
#include "PlatformConfig.h"
#include "CameraParameters.h"

#undef LOGD
#undef LOGI
#undef LOGW
#undef LOGE
#undef CLOGx
#undef _LOG_H_
#include "Logs.h"

#define VISUAL_WIDTH  286
#define SCREEN_WIDTH  1080
#define SCREEN_DP     40

namespace pandora {


PlatformOps::PlatformOps() :
    mModule(MODULE_PLATFORM_OPS)
{
    init();
}

void PlatformOps::init()
{
    mLastPlatformPrivSet.denoise = 0;
    mLastPlatformPrivSet.sharpen = 0;

    mParmValid[GET_PARM_COMMAND_INF] = false;
    mGetParms[GET_PARM_COMMAND_INF].commandInf.type =
        CommandInf::COMMAND_TYPE_LONGSHOT_OFF;
    mParmValid[GET_PARM_COMMAND_INF] = true;
}

bool PlatformOps::isInited()
{
    return true;
}

const char *PlatformOps::whoamI()
{
    return "ubuntu";
}

sp<ConfigInterface> &PlatformOps::getConfig()
{
    if (ISNULL(mConfig)) {
        mConfig = new Config();
    }

    return mConfig;
}

// ------------ SET PARMS START ------------
int32_t PlatformOps::setMultiShot(MultiShotParm &parm)
{
    return 0;
}

int32_t PlatformOps::setMultiExposure(MultiExporeParm &parm)
{
    return 0;
}

int32_t PlatformOps::setPlatformPrivSetting(PlatformPrivSet & /*parm*/)
{
    int32_t rc = NO_ERROR;

    return rc;
}

// ------------ OTHER TYPES START ------------
int32_t PlatformOps::sendEvtCallback(EvtCallbackInf &evt)
{
    return 0;
}

int32_t PlatformOps::sendDataCallback(DataCallbackInf &task)
{
    return 0;
}

// ------------ CAMERA STATUS INFO -----------
#define DEBUG_METADATA_PARMS 0

int32_t PlatformOps::onMetadataAvailable(const void *dat)
{
    int32_t rc = NO_ERROR;
    return rc;
}

int32_t PlatformOps::onParameterAvailable(const void *data)
{
    int32_t rc = NO_ERROR;

    const CameraParameters *parm =
        static_cast<const CameraParameters *>(data);

    RWLock::AutoWLock l(mParmLock);
    if (SUCCEED(rc)) {
        mParmValid[GET_PARM_BEAUTY_FACE] = false;
        int val = parm->get(KEY_BEAUTY_FACE);
        if (val >= 0) {
            mGetParms[GET_PARM_BEAUTY_FACE].beautySetting.mode = (BeautyMode)val;
            mGetParms[GET_PARM_BEAUTY_FACE].beautySetting.strength = 0;
            if (val == BEAUTY_MODE_MANUAL) {
                int val2 = parm->get(KEY_BEAUTY_FACE_LEVEL);
                mGetParms[GET_PARM_BEAUTY_FACE].beautySetting.strength = val2;
            }
            mParmValid[GET_PARM_BEAUTY_FACE] = true;
        }
    }

#if 0
    if (SUCCEED(rc)) {
        mParmValid[GET_PARM_FAIRLIGHT] = false;
        int val = parm->get(KEY_FAIRLIGHT_MODE);
        if (NOTNULL(val)) {
            if (!strcmp(val, FAIRLIGHT_VALUE_RAINBOW)) {
                mGetParms[GET_PARM_FAIRLIGHT].fairLightMode= FAIRLIGHT_RAINBOW;
            } else if (!strcmp(val, FAIRLIGHT_VALUE_MORNING)) {
                mGetParms[GET_PARM_FAIRLIGHT].fairLightMode = FAIRLIGHT_MORNING;
            } else if (!strcmp(val, FAIRLIGHT_VALUE_WAVE)) {
                mGetParms[GET_PARM_FAIRLIGHT].fairLightMode = FAIRLIGHT_WAVE;
            } else if (!strcmp(val, FAIRLIGHT_VALUE_CONTOUR)) {
                mGetParms[GET_PARM_FAIRLIGHT].fairLightMode = FAIRLIGHT_CONTOUR;
            } else if (!strcmp(val, FAIRLIGHT_VALUE_SHADOW)) {
                mGetParms[GET_PARM_FAIRLIGHT].fairLightMode = FAIRLIGHT_SHADOW;
            } else if (!strcmp(val, FAIRLIGHT_VALUE_STAGE)) {
                mGetParms[GET_PARM_FAIRLIGHT].fairLightMode = FAIRLIGHT_STAGE;
            } else {
                mGetParms[GET_PARM_FAIRLIGHT].fairLightMode = FAIRLIGHT_NONE;
            }
        }
        mParmValid[GET_PARM_FAIRLIGHT] = true;
    }
#endif
    if (SUCCEED(rc)) {
        mParmValid[GET_PARM_WATER_MARK] = true;
        mGetParms[GET_PARM_WATER_MARK].watermark.mode                = WATERMARK_MODE_ON;
        mGetParms[GET_PARM_WATER_MARK].watermark.dp                  = SCREEN_DP;
        mGetParms[GET_PARM_WATER_MARK].watermark.dpi                 = mDeviceDpi;
        mGetParms[GET_PARM_WATER_MARK].watermark.wmRatio.numerator   = VISUAL_WIDTH;
        mGetParms[GET_PARM_WATER_MARK].watermark.wmRatio.denominator = SCREEN_WIDTH;
        mGetParms[GET_PARM_WATER_MARK].watermark.product             = mProductName;
        mGetParms[GET_PARM_WATER_MARK].watermark.build               = mBuildVersion;
        mGetParms[GET_PARM_WATER_MARK].watermark.package             = mPackageName;
        mGetParms[GET_PARM_WATER_MARK].watermark.debug               = mDebugBuild;
        mGetParms[GET_PARM_WATER_MARK].watermark.id                  = 1;
        mGetParms[GET_PARM_WATER_MARK].watermark.camType             = mGetParms[GET_PARM_CAM_TYPE].camType;
        mGetParms[GET_PARM_WATER_MARK].watermark.enableLogo          = false;
        mGetParms[GET_PARM_WATER_MARK].watermark.enableTexts         = false;

        int val = parm->get(KEY_WATER_MARK);

        if(val >= 0) {
            mGetParms[GET_PARM_WATER_MARK].watermark.enableProduct = val;
        }
        mParmValid[GET_PARM_WATER_MARK] = true;
    }

    if(SUCCEED(rc)) {
        mParmValid[GET_PARM_PLATFORM_PRIV] = false;
        mGetParms[GET_PARM_PLATFORM_PRIV].platformPriv.scaleDim.w = 0;
        mGetParms[GET_PARM_PLATFORM_PRIV].platformPriv.scaleDim.h = 0;
        mParmValid[GET_PARM_PLATFORM_PRIV] = true;
    }
    return NO_ERROR;
}

int32_t PlatformOps::onCommandAvailable(int32_t cmd,
    int32_t /*arg1*/, int32_t /*arg2*/)
{
    int32_t rc = NO_ERROR;
    return rc;
}
};
