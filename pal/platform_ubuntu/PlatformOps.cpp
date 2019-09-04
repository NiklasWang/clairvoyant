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

    if (SUCCEED(rc)) {
        mParmValid[GET_PARM_BEAUTY_FACE] = false;
        const char *val = parm->get(CameraParameters::KEY_BEAUTY_FACE);
        if (NOTNULL(val)) {
            if (!strcmp(val, "auto")) {
                mGetParms[GET_PARM_BEAUTY_FACE].beautySetting.mode =
                    BEAUTY_MODE_ON;
                mGetParms[GET_PARM_BEAUTY_FACE].beautySetting.strength = 0;
            } else if (!strcmp(val, "manual")) {
                mGetParms[GET_PARM_BEAUTY_FACE].beautySetting.mode =
                    BEAUTY_MODE_MANUAL;
                const char *val2 = parm->get(CameraParameters::KEY_BEAUTY_FACE_LEVEL);
                mGetParms[GET_PARM_BEAUTY_FACE].beautySetting.strength =
                    NOTNULL(val2)? atoi(val2) : 0;
            } else {
                mGetParms[GET_PARM_BEAUTY_FACE].beautySetting.mode =
                    BEAUTY_MODE_OFF;
                mGetParms[GET_PARM_BEAUTY_FACE].beautySetting.strength = 0;
            }
            mParmValid[GET_PARM_BEAUTY_FACE] = true;
        }
    }

    if (SUCCEED(rc)) {
        mParmValid[GET_PARM_HDR_MODE] = false;
        const char *val = parm->get(CameraParameters::KEY_SCENE_MODE);
        if (NOTNULL(val)) {
            if (!strcmp(val, CameraParameters::SCENE_MODE_HDR)) {
                mGetParms[GET_PARM_HDR_MODE].hdrMode = HDR_MODE_FORCED_ON;
            } else if (!strcmp(val, CameraParameters::SCENE_MODE_ASD)) {
                mGetParms[GET_PARM_HDR_MODE].hdrMode = HDR_MODE_AUTO;
            } else {
                mGetParms[GET_PARM_HDR_MODE].hdrMode = HDR_MODE_FORCED_OFF;
            }
            mParmValid[GET_PARM_HDR_MODE] = true;
        }
    }

    if (SUCCEED(rc)) {
        mParmValid[GET_PARM_FLASH_MODE] = false;
        const char *val = parm->get(CameraParameters::KEY_FLASH_MODE);
        if (NOTNULL(val)) {
            if (!strcmp(val, CameraParameters::FLASH_MODE_ON)) {
                mGetParms[GET_PARM_FLASH_MODE].flashMode = FLASH_MODE_FORCED_ON;
            } else if (!strcmp(val, CameraParameters::FLASH_MODE_AUTO)) {
                mGetParms[GET_PARM_FLASH_MODE].flashMode = FLASH_MODE_AUTO;
            } else {
                mGetParms[GET_PARM_FLASH_MODE].flashMode = FLASH_MODE_FORCED_OFF;
            }
            mParmValid[GET_PARM_FLASH_MODE] = true;
        }
    }

    if (SUCCEED(rc)) {
        mParmValid[GET_PARM_LONG_SHOT] = false;
        const char *val = parm->get(CameraParameters::KEY_LONG_SHOT);
        if (NOTNULL(val)) {
            int32_t on = !strcmp(val, "on");
            mGetParms[GET_PARM_LONG_SHOT].longShot =
                on ? LONG_SHOT_MODE_ON : LONG_SHOT_MODE_OFF;
            mParmValid[GET_PARM_LONG_SHOT] = true;
        }
    }

    if (SUCCEED(rc)) {
        mParmValid[GET_PARM_SMART_SHOT] = false;
        const char *val = parm->get(CameraParameters::KEY_SMART_SHOT);
        if (NOTNULL(val)) {
            int32_t on = !strcmp(val, "on");
            mGetParms[GET_PARM_SMART_SHOT].smartShot =
                on ? SMART_SHOT_MODE_ON : SMART_SHOT_MODE_OFF;
            mParmValid[GET_PARM_SMART_SHOT] = true;
        }
    }

    if(SUCCEED(rc)){
        mParmValid[GET_PARM_VIDEO_STAB] = false;
        const char *val = parm->get(CameraParameters::KEY_MOVIE_SOLID);
        if (NOTNULL(val)) {
            int32_t on = !strcmp(val, "on");
            mGetParms[GET_PARM_VIDEO_STAB].videoStab =
                on ? VIDEO_STAB_MODE_VIDEO_ONLY : VIDEO_STAB_MODE_OFF;
            mParmValid[GET_PARM_VIDEO_STAB] = true;
        }
    }

    if(SUCCEED(rc)){
        mParmValid[GET_PARM_FLIP_MODE] = false;
        const char *val = parm->get(CameraParameters::KEY_VIDEO_PLIP);
        if (NOTNULL(val)) {
             if (!strcmp(val, "flip-v")) {
                mGetParms[GET_PARM_FLIP_MODE].flipMode.v = true;
             } else if (!strcmp(val, "flip-h")) {
                mGetParms[GET_PARM_FLIP_MODE].flipMode.h = true;
             } else if (!strcmp(val, "flip-vh")) {
                mGetParms[GET_PARM_FLIP_MODE].flipMode.v = true;
                mGetParms[GET_PARM_FLIP_MODE].flipMode.h = true;
             } else if(!strcmp(val, "flip-off")) {
                mGetParms[GET_PARM_FLIP_MODE].flipMode.v = false;
                mGetParms[GET_PARM_FLIP_MODE].flipMode.h = false;
             }
             mParmValid[GET_PARM_FLIP_MODE] = true;
        }
    }

    if(SUCCEED(rc)){
        mParmValid[GET_PARM_IMAGE_STAB] = false;
        const char *val = parm->get(CameraParameters::KEY_SCENE_MODE);
        if(NOTNULL(val)){
            int32_t on = !strcmp(val, CameraParameters::SCENE_MODE_NIGHT) ||
                !strcmp(val, CameraParameters::SCENE_MODE_NIGHT_PORTRAIT);
            mGetParms[GET_PARM_IMAGE_STAB].imageStab =
                on ? IMAGE_STAB_MODE_ON : IMAGE_STAB_MODE_OFF;
            mParmValid[GET_PARM_IMAGE_STAB] = true;
        }
    }

    if (SUCCEED(rc)) {
        mParmValid[GET_PARM_NIGHT_SHOT] = false;
        const char *val = parm->get(CameraParameters::KEY_NIGHT_SHOT);
        if (NOTNULL(val)) {
            if (!strcmp(val, "on")) {
                mGetParms[GET_PARM_NIGHT_SHOT].nightShot = NIGHT_SHOT_MODE_ON;
            } else if (!strcmp(val, "auto")) {
                mGetParms[GET_PARM_NIGHT_SHOT].nightShot = NIGHT_SHOT_MODE_AUTO;
            } else {
                mGetParms[GET_PARM_NIGHT_SHOT].nightShot = NIGHT_SHOT_MODE_OFF;
            }
            mParmValid[GET_PARM_NIGHT_SHOT] = true;
        }
    }

    if(SUCCEED(rc)){
        mParmValid[GET_PARM_LONG_EXPOSURE] = false;
        const char *val = parm->get(CameraParameters::KEY_LONG_EXPOSURE);
        if(NOTNULL(val)){
            int32_t on = !strcmp(val, "on");
            mGetParms[GET_PARM_LONG_EXPOSURE].longExposure.on = on;
            mGetParms[GET_PARM_LONG_EXPOSURE].longExposure.time =
                on ? 0.0f : 0.0f; // TODO: impl long exposure here if necessary
            mParmValid[GET_PARM_LONG_EXPOSURE] = true;
        }
    }

    if (SUCCEED(rc)) {
        mParmValid[GET_PARM_DUAL_CAM_MODE] = false;
        mGetParms[GET_PARM_DUAL_CAM_MODE].dualCamMode.funcMask = DUAL_CAMERA_MASK_NONE_FUNC;
        mParmValid[GET_PARM_DUAL_CAM_MODE] = true;
    }

    if (SUCCEED(rc)) {
        mParmValid[GET_PARM_FAIRLIGHT] = false;
        const char *val = parm->get(CameraParameters::KEY_FAIRLIGHT_MODE);
        if (NOTNULL(val)) {
            if (!strcmp(val, CameraParameters::FAIRLIGHT_VALUE_RAINBOW)) {
                mGetParms[GET_PARM_FAIRLIGHT].fairLightMode= FAIRLIGHT_RAINBOW;
            } else if (!strcmp(val, CameraParameters::FAIRLIGHT_VALUE_MORNING)) {
                mGetParms[GET_PARM_FAIRLIGHT].fairLightMode = FAIRLIGHT_MORNING;
            } else if (!strcmp(val, CameraParameters::FAIRLIGHT_VALUE_WAVE)) {
                mGetParms[GET_PARM_FAIRLIGHT].fairLightMode = FAIRLIGHT_WAVE;
            } else if (!strcmp(val, CameraParameters::FAIRLIGHT_VALUE_CONTOUR)) {
                mGetParms[GET_PARM_FAIRLIGHT].fairLightMode = FAIRLIGHT_CONTOUR;
            } else if (!strcmp(val, CameraParameters::FAIRLIGHT_VALUE_SHADOW)) {
                mGetParms[GET_PARM_FAIRLIGHT].fairLightMode = FAIRLIGHT_SHADOW;
            } else if (!strcmp(val, CameraParameters::FAIRLIGHT_VALUE_STAGE)) {
                mGetParms[GET_PARM_FAIRLIGHT].fairLightMode = FAIRLIGHT_STAGE;
            } else {
                mGetParms[GET_PARM_FAIRLIGHT].fairLightMode = FAIRLIGHT_NONE;
            }
        }
        mParmValid[GET_PARM_FAIRLIGHT] = true;
    }

    if (SUCCEED(rc)) {
        mParmValid[GET_PARM_ROTATION] = false;
        int32_t rotation = parm->getInt(CameraParameters::KEY_ROTATION);
        if (rotation != -1) {
            switch (rotation) {
                case 0:
                    mGetParms[GET_PARM_ROTATION].rotation = ROTATION_ANGLE_0;
                    mParmValid[GET_PARM_ROTATION] = true;
                    break;
                case 90:
                    mGetParms[GET_PARM_ROTATION].rotation = ROTATION_ANGLE_90;
                    mParmValid[GET_PARM_ROTATION] = true;
                    break;
                case 180:
                    mGetParms[GET_PARM_ROTATION].rotation = ROTATION_ANGLE_180;
                    mParmValid[GET_PARM_ROTATION] = true;
                    break;
                case 270:
                    mGetParms[GET_PARM_ROTATION].rotation = ROTATION_ANGLE_270;
                    mParmValid[GET_PARM_ROTATION] = true;
                    break;
                default:
                    LOGE(mModule, "The rotation %d is invalid", rotation);
            }
        }
        mParmValid[GET_PARM_ROTATION] = true;
    }

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

        const char *val = parm->get(CameraParameters::KEY_WATER_MARK);
        if(NOTNULL(val)) {
            int32_t on = !strcmp(val, CameraParameters::ON);
            if (on) {
                mGetParms[GET_PARM_WATER_MARK].watermark.enableProduct = true;
            } else {
                mGetParms[GET_PARM_WATER_MARK].watermark.enableProduct = false;
            }
        }
    }

    if(SUCCEED(rc)) {
        mParmValid[GET_PARM_PLATFORM_PRIV] = false;
        mGetParms[GET_PARM_PLATFORM_PRIV].platformPriv.scaleDim.w = 0;
        mGetParms[GET_PARM_PLATFORM_PRIV].platformPriv.scaleDim.h = 0;
        mParmValid[GET_PARM_PLATFORM_PRIV] = true;
    }

    if (SUCCEED(rc)) {
        SingleBokehMode on = SINGLE_CAM_BOKEH_OFF;
        int32_t blurLevel = 0;
        mParmValid[GET_PARM_SINGLE_BOKEH] = false;
        const char *val1 = parm->get(CameraParameters::KEY_QC_SINGLE_BOKEH);
        const char *val2 = parm->get(CameraParameters::KEY_QC_SINGLE_BOKEH_LEVEL);
        on = NOTNULL(val1) ? ((atoi(val1) == 1) ? SINGLE_CAM_BOKEH_ON : on) : on;
        blurLevel = NOTNULL(val2) ? atoi(val2) : blurLevel;
        mGetParms[GET_PARM_SINGLE_BOKEH].singleBokeh.mode = on;
        mGetParms[GET_PARM_SINGLE_BOKEH].singleBokeh.blurLevel = blurLevel;
        mParmValid[GET_PARM_SINGLE_BOKEH] = true;
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
