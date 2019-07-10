#include "Common.h"
#include "PlatformConfig.h"

namespace pandora {

const ConfigInterface::ConfigSettings Config::gSettings[] = {
    {   // Camera 0, back camera
        .focusEnd = {
            .macroEndRatio    = 50.0,
            .infiniteEndRatio = 101.0,
        },
        .expChange = {
            .luxChangeThres      = 102400,
            .isoChangeThres      = 400,
            .exposureChangeThres = 20,
            .apertureChangeThres = 9,
        },
        .nightStab = {
            .luxThre       = 1024,
            .isoThres      = 1000,
            .exposureThres = 1.0,
            .apertureThres = 1.0,
        },
        .lowLight = {
            .luxThre       = 1024,
            .isoThres      = 1200,
            .exposureThres = 1.0,
            .apertureThres = 1.0,
        },
        .hdrConf  = 0.4,
        .extendId = 0x1FFF,
    },
    {   // Camera 1, front camera
        .focusEnd = {
            .macroEndRatio    = 50.0,
            .infiniteEndRatio = 101.0,
        },
        .expChange = {
            .luxChangeThres      = 102400,
            .isoChangeThres      = 400,
            .exposureChangeThres = 20,
            .apertureChangeThres = 9,
        },
        .nightStab = {
            .luxThre       = 1024,
            .isoThres      = 1600,
            .exposureThres = 1.0,
            .apertureThres = 1.0,
        },
        .lowLight = {
            .luxThre       = 1024,
            .isoThres      = 600,
            .exposureThres = 1.0,
            .apertureThres = 1.0,
        },
        .hdrConf  = 0.4,
        .extendId = 0x1FFF,
    },
    {   // Camera 2, back mono camera
        .focusEnd = {
            .macroEndRatio    = 50.0,
            .infiniteEndRatio = 101.0,
        },
        .expChange = {
            .luxChangeThres      = 102400,
            .isoChangeThres      = 400,
            .exposureChangeThres = 20,
            .apertureChangeThres = 9,
        },
        .nightStab = {
            .luxThre       = 1024,
            .isoThres      = 102400,
            .exposureThres = 1.0,
            .apertureThres = 1.0,
        },
        .lowLight = {
            .luxThre       = 1024,
            .isoThres      = 102400,
            .exposureThres = 1.0,
            .apertureThres = 1.0,
        },
        .hdrConf  = 0.4,
        .extendId = 0x1FFF,
    },
    {   // Camera 3, back camera, 2nd vendor
        .focusEnd = {
            .macroEndRatio    = 50.0,
            .infiniteEndRatio = 101.0,
        },
        .expChange = {
            .luxChangeThres      = 102400,
            .isoChangeThres      = 400,
            .exposureChangeThres = 20,
            .apertureChangeThres = 9,
        },
        .nightStab = {
            .luxThre       = 1024,
            .isoThres      = 1200,
            .exposureThres = 1.0,
            .apertureThres = 1.0,
        },
        .lowLight = {
            .luxThre       = 1024,
            .isoThres      = 1200,
            .exposureThres = 1.0,
            .apertureThres = 1.0,
        },
        .hdrConf  = 0.4,
        .extendId = 0x1FFF,
    },
};

#define SET_PARM_CATEGORY(TYPE, GSCO, PARM, CAT) \
    { \
        .type = TYPE, \
        .u = { \
            .GSCO  = PARM, \
        }, \
        .category = CAT, \
    },

const ParmCategory Config::gParmCategory[] = {
    // Get parms
    SET_PARM_CATEGORY(PARM_TYPE_GET, g, GET_PARM_PREVIEW_SIZE,  PARM_CATEGORY_TYPE_1)
    SET_PARM_CATEGORY(PARM_TYPE_GET, g, GET_PARM_PICTURE_SIZE,  PARM_CATEGORY_TYPE_1)
    SET_PARM_CATEGORY(PARM_TYPE_GET, g, GET_PARM_VIDEO_SIZE,    PARM_CATEGORY_TYPE_1)
    SET_PARM_CATEGORY(PARM_TYPE_GET, g, GET_PARM_FLASH_MODE,    PARM_CATEGORY_TYPE_1)
    SET_PARM_CATEGORY(PARM_TYPE_GET, g, GET_PARM_HDR_MODE,      PARM_CATEGORY_TYPE_1)
    SET_PARM_CATEGORY(PARM_TYPE_GET, g, GET_PARM_LONG_SHOT,     PARM_CATEGORY_TYPE_1)
    SET_PARM_CATEGORY(PARM_TYPE_GET, g, GET_PARM_LONG_EXPOSURE, PARM_CATEGORY_TYPE_1)
    SET_PARM_CATEGORY(PARM_TYPE_GET, g, GET_PARM_SMART_SHOT,    PARM_CATEGORY_TYPE_1)
    SET_PARM_CATEGORY(PARM_TYPE_GET, g, GET_PARM_IMAGE_STAB,    PARM_CATEGORY_TYPE_1)
    SET_PARM_CATEGORY(PARM_TYPE_GET, g, GET_PARM_VIDEO_STAB,    PARM_CATEGORY_TYPE_1)
    SET_PARM_CATEGORY(PARM_TYPE_GET, g, GET_PARM_FLIP_MODE,     PARM_CATEGORY_TYPE_1)
    SET_PARM_CATEGORY(PARM_TYPE_GET, g, GET_PARM_NIGHT_SHOT,    PARM_CATEGORY_TYPE_1)
    SET_PARM_CATEGORY(PARM_TYPE_GET, g, GET_PARM_BEAUTY_FACE,   PARM_CATEGORY_TYPE_1)
    SET_PARM_CATEGORY(PARM_TYPE_GET, g, GET_PARM_FACE_INFO,     PARM_CATEGORY_TYPE_2)
    SET_PARM_CATEGORY(PARM_TYPE_GET, g, GET_PARM_AE_INFO,       PARM_CATEGORY_TYPE_2)
    SET_PARM_CATEGORY(PARM_TYPE_GET, g, GET_PARM_AWB_INFO,      PARM_CATEGORY_TYPE_2)
    SET_PARM_CATEGORY(PARM_TYPE_GET, g, GET_PARM_AF_INFO,       PARM_CATEGORY_TYPE_2)
    SET_PARM_CATEGORY(PARM_TYPE_GET, g, GET_PARM_HDR_INFO,      PARM_CATEGORY_TYPE_2)
    SET_PARM_CATEGORY(PARM_TYPE_GET, g, GET_PARM_SCENE_INFO,    PARM_CATEGORY_TYPE_2)
    SET_PARM_CATEGORY(PARM_TYPE_GET, g, GET_PARM_CAM_TYPE,      PARM_CATEGORY_TYPE_3)
    SET_PARM_CATEGORY(PARM_TYPE_GET, g, GET_PARM_COMMAND_INF,   PARM_CATEGORY_TYPE_4)
    SET_PARM_CATEGORY(PARM_TYPE_GET, g, GET_PARM_DUAL_CAM_MODE, PARM_CATEGORY_TYPE_2)
    SET_PARM_CATEGORY(PARM_TYPE_GET, g, GET_PARM_SUB_CAM_DATA,  PARM_CATEGORY_TYPE_2)
    SET_PARM_CATEGORY(PARM_TYPE_GET, g, GET_PARM_FAIRLIGHT,     PARM_CATEGORY_TYPE_2)
    SET_PARM_CATEGORY(PARM_TYPE_GET, g, GET_PARM_ROTATION,      PARM_CATEGORY_TYPE_2)
    SET_PARM_CATEGORY(PARM_TYPE_GET, g, GET_PARM_WATER_MARK,    PARM_CATEGORY_TYPE_2)
    SET_PARM_CATEGORY(PARM_TYPE_GET, g, GET_PARM_ZOOM_INF,      PARM_CATEGORY_TYPE_1)
    SET_PARM_CATEGORY(PARM_TYPE_GET, g, GET_PARM_SINGLE_BOKEH,  PARM_CATEGORY_TYPE_1)
    SET_PARM_CATEGORY(PARM_TYPE_GET, g, GET_PARM_PLATFORM_PRIV, PARM_CATEGORY_TYPE_2)
    // Set parms
    SET_PARM_CATEGORY(PARM_TYPE_SET, s, SET_PARM_MULTI_SHOT,    PARM_CATEGORY_TYPE_5)
    SET_PARM_CATEGORY(PARM_TYPE_SET, s, SET_PARM_AE_BRACKET,    PARM_CATEGORY_TYPE_5)
    SET_PARM_CATEGORY(PARM_TYPE_SET, s, SET_PARM_PLATFORM_PRIV, PARM_CATEGORY_TYPE_5)
    // Configs
    SET_PARM_CATEGORY(PARM_TYPE_CONFIG, c, CONFIG_FOCUS_END_THRES,  PARM_CATEGORY_TYPE_FAST)
    SET_PARM_CATEGORY(PARM_TYPE_CONFIG, c, CONFIG_EXPOSURE_CHANGE,  PARM_CATEGORY_TYPE_FAST)
    SET_PARM_CATEGORY(PARM_TYPE_CONFIG, c, CONFIG_NIGHT_STAB_THRES, PARM_CATEGORY_TYPE_FAST)
    SET_PARM_CATEGORY(PARM_TYPE_CONFIG, c, CONFIG_HDR_CONFI_THRES,  PARM_CATEGORY_TYPE_FAST)
    SET_PARM_CATEGORY(PARM_TYPE_CONFIG, c, CONFIG_EXTENDED_MSGID,   PARM_CATEGORY_TYPE_FAST)
    SET_PARM_CATEGORY(PARM_TYPE_CONFIG, c, CONFIG_LOW_LIGHT_THRES,  PARM_CATEGORY_TYPE_FAST)
    SET_PARM_CATEGORY(PARM_TYPE_CONFIG, c, CONFIG_PARM_CATEGORY,    PARM_CATEGORY_TYPE_FAST)
    // Others
    SET_PARM_CATEGORY(PARM_TYPE_OTHERS, o, OTHER_TYPE_EVT_CALLBACK,  PARM_CATEGORY_TYPE_FAST)
    SET_PARM_CATEGORY(PARM_TYPE_OTHERS, o, OTHER_TYPE_DATA_CALLBACK, PARM_CATEGORY_TYPE_FAST)
};

Config::Config(uint32_t camId) :
    ConfigInterface(camId)
{
    if (mCamId >= ARRAYSIZE(gSettings)) {
        LOGE(mModule, "Invalid camera id %d/%d",
            mCamId, ARRAYSIZE(gSettings));
    }
}

bool Config::isInited()
{
    return true;
}

#define BEGIN_CHECK_INIT() \
    int32_t __rc = NO_ERROR; \
    if (!isInited()) { \
        __rc = NOT_INITED; \
    } \
    if (SUCCEED(__rc)) { \

#define FINISH_CHECK_INIT() \
    }

#define GET_CONFIG(lhs, rhs) \
    ({ \
        BEGIN_CHECK_INIT(); \
        lhs = gSettings[mCamId].rhs; \
        FINISH_CHECK_INIT(); \
        __rc; \
    })

int32_t Config::getFocusEndThres(FocusEndConfig &conf)
{
    return GET_CONFIG(conf, focusEnd);
}

int32_t Config::getExposureChangeThres(ExpChangeConfig &conf)
{
    return GET_CONFIG(conf, expChange);
}

int32_t Config::getNightStabilizerThres(NightStabConfig &conf)
{
    return GET_CONFIG(conf, nightStab);
}

int32_t Config::getHdrConfidenceThres(float &conf)
{
    return GET_CONFIG(conf, hdrConf);
}

int32_t Config::getExtendedMsgID(int32_t &conf)
{
    return GET_CONFIG(conf, extendId);
}

int32_t Config::getLowLightThres(LowLightConfig &conf)
{
    return GET_CONFIG(conf, lowLight);
}

inline int32_t getAllParmSize()
{
    return GET_PARM_MAX_INVALID + SET_PARM_MAX_INVALID +
        CONFIG_MAX_INVALID + OTHER_TYPE_MAX_INVALID;
}

int32_t Config::getParmCategory(ParmsCategory &result)
{
    ASSERT_LOG(mModule,
        ARRAYSIZE(gParmCategory) == getAllParmSize(),
        "Setting and array size not match %d/%d.",
        ARRAYSIZE(gParmCategory), getAllParmSize());

    BEGIN_CHECK_INIT();
    result.size = ARRAYSIZE(gParmCategory);
    const ParmCategory *category = gParmCategory;
    result.data = const_cast<ParmCategory *>(category);
    FINISH_CHECK_INIT();
    return __rc;
}

};

