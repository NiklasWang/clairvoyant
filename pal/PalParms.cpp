#include "PalParms.h"

namespace pandora {

PalParms::PalParms() :
    type(PARM_TYPE_MAX_INVALID)
{
}

template<>
const PalParmType &operator<< (PalParms &parm, const PalParmType &rhs)
{
    parm.type = rhs;
    return rhs;
}

template<>
const PalParms &operator>> (const PalParms &parm, PalParmType &rhs)
{
    rhs = parm.type;
    return parm;
}

#define DEFINE_OPERATOR_IN(T, TYPE, ELE) \
    template<> \
    const T &operator<< (PalParms &parm, const T &rhs) \
    { \
        parm<<TYPE; \
        parm.ELE = rhs; \
        return rhs; \
    }

#define DEFINE_OPERATOR_OUT(T, ELE) \
    template<> \
    const PalParms &operator>> (const PalParms &parm, T &rhs) \
    { \
        rhs = parm.ELE; \
        return parm; \
    }

#define DEFINE_OPERATOR_IN_OUT(T, TYPE, ELE) \
    DEFINE_OPERATOR_IN(T, TYPE, ELE) \
    DEFINE_OPERATOR_OUT(T, ELE)


DEFINE_OPERATOR_IN_OUT(GetParamType,   PARM_TYPE_GET,          g.type);
DEFINE_OPERATOR_IN_OUT(PreviewDim,     GET_PARM_PREVIEW_SIZE,  g.u.previewSize);
DEFINE_OPERATOR_IN_OUT(PictureDim,     GET_PARM_PICTURE_SIZE,  g.u.pictureSize);
DEFINE_OPERATOR_IN_OUT(VideoDim,       GET_PARM_VIDEO_SIZE,    g.u.videoSize);
DEFINE_OPERATOR_IN_OUT(FlashMode,      GET_PARM_FLASH_MODE,    g.u.flashMode);
DEFINE_OPERATOR_IN_OUT(HdrMode,        GET_PARM_HDR_MODE,      g.u.hdrMode);
DEFINE_OPERATOR_IN_OUT(LongshotMode,   GET_PARM_LONG_SHOT,     g.u.longShot);
DEFINE_OPERATOR_IN_OUT(LongExpMode,    GET_PARM_LONG_EXPOSURE, g.u.longExposure);
DEFINE_OPERATOR_IN_OUT(SmartshotMode,  GET_PARM_SMART_SHOT,    g.u.smartShot);
DEFINE_OPERATOR_IN_OUT(ImageStabMode,  GET_PARM_IMAGE_STAB,    g.u.imageStab);
DEFINE_OPERATOR_IN_OUT(VideoStabMode,  GET_PARM_VIDEO_STAB,    g.u.videoStab);
DEFINE_OPERATOR_IN_OUT(FlipMode,       GET_PARM_FLIP_MODE,     g.u.flipMode);
DEFINE_OPERATOR_IN_OUT(NightShotMode,  GET_PARM_NIGHT_SHOT,    g.u.nightShot);
DEFINE_OPERATOR_IN_OUT(BeautySetting,  GET_PARM_BEAUTY_FACE,   g.u.beautySetting);
DEFINE_OPERATOR_IN_OUT(FaceInfo,       GET_PARM_FACE_INFO,     g.u.faces);
DEFINE_OPERATOR_IN_OUT(AEInfo,         GET_PARM_AE_INFO,       g.u.ae);
DEFINE_OPERATOR_IN_OUT(AWBInfo,        GET_PARM_AWB_INFO,      g.u.awb);
DEFINE_OPERATOR_IN_OUT(AFInfo,         GET_PARM_AF_INFO,       g.u.af);
DEFINE_OPERATOR_IN_OUT(HdrInfo,        GET_PARM_HDR_INFO,      g.u.hdr);
DEFINE_OPERATOR_IN_OUT(SceneInfo,      GET_PARM_SCENE_INFO,    g.u.scene);
DEFINE_OPERATOR_IN_OUT(CameraTypeInf,  GET_PARM_CAM_TYPE,      g.u.camType);
DEFINE_OPERATOR_IN_OUT(CommandInf,     GET_PARM_COMMAND_INF,   g.u.commandInf);
DEFINE_OPERATOR_IN_OUT(DualCamMode,    GET_PARM_DUAL_CAM_MODE, g.u.dualCamMode);
DEFINE_OPERATOR_IN_OUT(SubCamData,     GET_PARM_SUB_CAM_DATA,  g.u.subCamData);
DEFINE_OPERATOR_IN_OUT(FairLightMode,  GET_PARM_FAIRLIGHT,     g.u.fairLightMode);
DEFINE_OPERATOR_IN_OUT(RotationAngle,  GET_PARM_ROTATION,      g.u.rotation);
DEFINE_OPERATOR_IN_OUT(WaterMarkInfo,  GET_PARM_WATER_MARK,    g.u.watermark);
DEFINE_OPERATOR_IN_OUT(ZoomInf,        GET_PARM_ZOOM_INF,      g.u.zoom);
DEFINE_OPERATOR_IN_OUT(SingleBokehInf, GET_PARM_SINGLE_BOKEH,  g.u.singleBokeh);
DEFINE_OPERATOR_IN_OUT(PlatformPriv,   GET_PARM_PLATFORM_PRIV, g.u.platformPriv);


DEFINE_OPERATOR_IN_OUT(SetParamType,    PARM_TYPE_SET,        s.type);
DEFINE_OPERATOR_IN_OUT(MultiShotParm,   SET_PARM_MULTI_SHOT,  s.u.multiShot);
DEFINE_OPERATOR_IN_OUT(MultiExporeParm, SET_PARM_AE_BRACKET,  s.u.multiExposure);
DEFINE_OPERATOR_IN_OUT(PlatformPrivSet, SET_PARM_PLATFORM_PRIV,  s.u.platformPriv);


DEFINE_OPERATOR_IN_OUT(ConfigType,      PARM_TYPE_CONFIG,        c.type);
DEFINE_OPERATOR_IN_OUT(ConfigInterface::ThresType, CONFIG_HDR_CONFI_THRES,  c.u.hdrConfiThres);
DEFINE_OPERATOR_IN_OUT(ConfigInterface::IdType,    CONFIG_EXTENDED_MSGID,   c.u.extMsgID);
DEFINE_OPERATOR_IN_OUT(FocusEndConfig,  CONFIG_FOCUS_END_THRES,  c.u.focusEnd);
DEFINE_OPERATOR_IN_OUT(ExpChangeConfig, CONFIG_EXPOSURE_CHANGE,  c.u.expChange);
DEFINE_OPERATOR_IN_OUT(NightStabConfig, CONFIG_NIGHT_STAB_THRES, c.u.nightStabThres);
DEFINE_OPERATOR_IN_OUT(LowLightConfig,  CONFIG_LOW_LIGHT_THRES,  c.u.lowlight);
DEFINE_OPERATOR_IN_OUT(ParmsCategory,   CONFIG_PARM_CATEGORY,    c.u.parmCategory);


DEFINE_OPERATOR_IN_OUT(OtherType,       PARM_TYPE_OTHERS,         o.type);
DEFINE_OPERATOR_IN_OUT(EvtCallbackInf,  OTHER_TYPE_EVT_CALLBACK,  o.u.evt);
DEFINE_OPERATOR_IN_OUT(DataCallbackInf, OTHER_TYPE_DATA_CALLBACK, o.u.data);

const char *const ParmTypeString[] = {
    [PARM_TYPE_GET]          = "PARM_TYPE_GET",
    [PARM_TYPE_SET]          = "PARM_TYPE_SET",
    [PARM_TYPE_CONFIG]       = "PARM_TYPE_CONFIG",
    [PARM_TYPE_OTHERS]       = "PARM_TYPE_OTHERS",
    [PARM_TYPE_MAX_INVALID]  = "PARM_TYPE_MAX_INVALID",
};

const char *const GetParmTypeString[] = {
    [GET_PARM_PREVIEW_SIZE]  = "GET_PARM_PREVIEW_SIZE",
    [GET_PARM_PICTURE_SIZE]  = "GET_PARM_PICTURE_SIZE",
    [GET_PARM_VIDEO_SIZE]    = "GET_PARM_VIDEO_SIZE",
    [GET_PARM_FLASH_MODE]    = "GET_PARM_FLASH_MODE",
    [GET_PARM_HDR_MODE]      = "GET_PARM_HDR_MODE",
    [GET_PARM_LONG_SHOT]     = "GET_PARM_LONG_SHOT",
    [GET_PARM_LONG_EXPOSURE] = "GET_PARM_LONG_EXPOSURE",
    [GET_PARM_SMART_SHOT]    = "GET_PARM_SMART_SHOT",
    [GET_PARM_IMAGE_STAB]    = "GET_PARM_IMAGE_STAB",
    [GET_PARM_VIDEO_STAB]    = "GET_PARM_VIDEO_STAB",
    [GET_PARM_FLIP_MODE]     = "GET_PARM_FLIP_MODE",
    [GET_PARM_NIGHT_SHOT]    = "GET_PARM_NIGHT_SHOT",
    [GET_PARM_BEAUTY_FACE]   = "GET_PARM_BEAUTY_FACE",
    [GET_PARM_FACE_INFO]     = "GET_PARM_FACE_INFO",
    [GET_PARM_AE_INFO]       = "GET_PARM_AE_INFO",
    [GET_PARM_AWB_INFO]      = "GET_PARM_AWB_INFO",
    [GET_PARM_AF_INFO]       = "GET_PARM_AF_INFO",
    [GET_PARM_HDR_INFO]      = "GET_PARM_HDR_INFO",
    [GET_PARM_SCENE_INFO]    = "GET_PARM_SCENE_INFO",
    [GET_PARM_CAM_TYPE]      = "GET_PARM_CAM_TYPE",
    [GET_PARM_COMMAND_INF]   = "GET_PARM_COMMAND_INF",
    [GET_PARM_DUAL_CAM_MODE] = "GET_PARM_DUAL_CAMERA_MODE",
    [GET_PARM_SUB_CAM_DATA]  = "GET_PARM_SUB_CAMERA_DATA",
    [GET_PARM_FAIRLIGHT]     = "GET_PARM_FAIRLIGHT",
    [GET_PARM_ROTATION]      = "GET_PARM_ROTATION",
    [GET_PARM_WATER_MARK]    = "GET_PARM_WATER_MARK",
    [GET_PARM_ZOOM_INF]      = "GET_PARM_ZOOM_INF",
    [GET_PARM_SINGLE_BOKEH]  = "GET_PARM_SINGLE_BOKEH",
    [GET_PARM_PLATFORM_PRIV] = "GET_PARM_PLATFORM_PRIVATE",
    [GET_PARM_MAX_INVALID]   = "GET_PARM_MAX_INVALID",
};

const char *const SetParmTypeString[] = {
    [SET_PARM_MULTI_SHOT]    = "SET_PARM_MULTI_SHOT",
    [SET_PARM_AE_BRACKET]    = "SET_PARM_AE_BRACKET",
    [SET_PARM_PLATFORM_PRIV] = "SET_PARM_PLATFORM_PRIV",
    [SET_PARM_MAX_INVALID]   = "SET_PARM_MAX_INVALID",
};

const char *const ConfigTypeString[] = {
    [CONFIG_FOCUS_END_THRES]  = "CONFIG_FOCUS_END_THRES",
    [CONFIG_EXPOSURE_CHANGE]  = "CONFIG_EXPOSURE_CHANGE",
    [CONFIG_NIGHT_STAB_THRES] = "CONFIG_NIGHT_STAB_THRES",
    [CONFIG_HDR_CONFI_THRES]  = "CONFIG_HDR_CONFI_THRES",
    [CONFIG_EXTENDED_MSGID]   = "CONFIG_EXTENDED_MSGID",
    [CONFIG_LOW_LIGHT_THRES]  = "CONFIG_LOW_LIGHT_THRES",
    [CONFIG_PARM_CATEGORY]    = "CONFIG_PARM_CATEGORY",
    [CONFIG_MAX_INVALID]      = "CONFIG_MAX_INVALID",
};

const char *const OtherTypeString[] = {
    [OTHER_TYPE_EVT_CALLBACK]  = "OTHER_TYPE_EVT_CALLBACK",
    [OTHER_TYPE_DATA_CALLBACK] = "OTHER_THYP_DATA_CALLBACK",
    [OTHER_TYPE_MAX_INVALID]   = "OTHER_TYPE_MAX_INVALID",
};

#define GET_NAME(strArray, maxInvalid, type) \
    ({ \
        if (type > maxInvalid || type < 0) { \
            type = maxInvalid; \
        } \
        \
        strArray[type]; \
    })

#define DEFINE_GET_NAME_FUNC(parmType, strArray, maxInvalid) \
    template<> \
    const char *PalParms::getName<parmType>(parmType type) \
    { \
        return GET_NAME(strArray, maxInvalid, type); \
    }

DEFINE_GET_NAME_FUNC(PalParmType,  ParmTypeString,    PARM_TYPE_MAX_INVALID);
DEFINE_GET_NAME_FUNC(GetParamType, GetParmTypeString, GET_PARM_MAX_INVALID);
DEFINE_GET_NAME_FUNC(SetParamType, SetParmTypeString, SET_PARM_MAX_INVALID);
DEFINE_GET_NAME_FUNC(ConfigType,   ConfigTypeString,  CONFIG_MAX_INVALID);
DEFINE_GET_NAME_FUNC(OtherType,    OtherTypeString,   OTHER_TYPE_MAX_INVALID);

const char *PalParms::getName()
{
    const char *result = NULL;

    switch (type) {
        case PARM_TYPE_GET: {
            result = getName(g.type);
        } break;
        case PARM_TYPE_SET: {
            result = getName(s.type);
        } break;
        case PARM_TYPE_CONFIG: {
            result = getName(c.type);
        } break;
        case PARM_TYPE_OTHERS: {
            result = getName(o.type);
        } break;
        case PARM_TYPE_MAX_INVALID:
        default: {
            result = getName(type);
        } break;
    }

    return result;
}

const char *PalParms::whoamI()
{
    return getName();
}

};

