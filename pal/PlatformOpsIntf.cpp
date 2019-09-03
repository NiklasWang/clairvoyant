#include "Common.h"
#include "PlatformOpsIntf.h"

namespace pandora {

PlatformOpsIntf::PlatformOpsIntf() :
    mConfig(NULL)
{
    for (uint32_t i = 0; i < GET_PARM_MAX_INVALID; i++) {
        mParmValid[i] = false;
    }

    memset(mGetParms, 0,
        GET_PARM_MAX_INVALID * sizeof(PalParms::_g::_u));
}

bool PlatformOpsIntf::isInited()
{
    return false;
}

#define REQUEST_HANDLER_CASE(item, func, ele) \
    case item: { \
        rc = func(request->ele); \
    } break;

#define REQUEST_HANDLER_DEFAULT(ele, msg) \
    default: { \
        LOGE(MODULE_PAL_IMPL, "Invalid %s type %d at line %d", \
            msg, request->ele, __LINE__); \
        rc = PARAM_INVALID; \
    } break;

int32_t PlatformOpsIntf::requestHandler(PalParms *request)
{
    int32_t rc = PARAM_INVALID;
    ASSERT_LOG(MODULE_PLATFORM_OPS, NOTNULL(request), "request shouldn't be NULL");
    LOGE(MODULE_PAL_IMPL, "request->g.type %d", request->g.type);

    switch (request->type) {
        case PARM_TYPE_GET: {
            switch (request->g.type) {
                REQUEST_HANDLER_CASE(GET_PARM_PREVIEW_SIZE,   getPreviewDim,     g.u.previewSize);
                REQUEST_HANDLER_CASE(GET_PARM_PICTURE_SIZE,   getPictureDim,     g.u.pictureSize);
                REQUEST_HANDLER_CASE(GET_PARM_VIDEO_SIZE,     getVideoDim,       g.u.videoSize);
                REQUEST_HANDLER_CASE(GET_PARM_FLASH_MODE,     getFlashMode,      g.u.flashMode);
                REQUEST_HANDLER_CASE(GET_PARM_HDR_MODE,       getHdrMode,        g.u.hdrMode);
                REQUEST_HANDLER_CASE(GET_PARM_LONG_SHOT,      getLongshotMode,   g.u.longShot);
                REQUEST_HANDLER_CASE(GET_PARM_LONG_EXPOSURE,  getLongExpMode,    g.u.longExposure);
                REQUEST_HANDLER_CASE(GET_PARM_SMART_SHOT,     getSmartshotMode,  g.u.smartShot);
                REQUEST_HANDLER_CASE(GET_PARM_IMAGE_STAB,     getImageStabMode,  g.u.imageStab);
                REQUEST_HANDLER_CASE(GET_PARM_VIDEO_STAB,     getVideoStabMode,  g.u.videoStab);
                REQUEST_HANDLER_CASE(GET_PARM_FLIP_MODE,      getFlipMode,       g.u.flipMode);
                REQUEST_HANDLER_CASE(GET_PARM_NIGHT_SHOT,     getNightShotMode,  g.u.nightShot);
                REQUEST_HANDLER_CASE(GET_PARM_BEAUTY_FACE,    getBeautySetting,  g.u.beautySetting);
                REQUEST_HANDLER_CASE(GET_PARM_FACE_INFO,      getFaceInfo,       g.u.faces);
                REQUEST_HANDLER_CASE(GET_PARM_AE_INFO,        getAEInfo,         g.u.ae);
                REQUEST_HANDLER_CASE(GET_PARM_AWB_INFO,       getAWBInfo,        g.u.awb);
                REQUEST_HANDLER_CASE(GET_PARM_AF_INFO,        getAFInfo,         g.u.af);
                REQUEST_HANDLER_CASE(GET_PARM_HDR_INFO,       getHdrInfo,        g.u.hdr);
                REQUEST_HANDLER_CASE(GET_PARM_SCENE_INFO,     getSceneInfo,      g.u.scene);
                REQUEST_HANDLER_CASE(GET_PARM_CAM_TYPE,       getCameraTypeInf,  g.u.camType);
                REQUEST_HANDLER_CASE(GET_PARM_COMMAND_INF,    getCommandInf,     g.u.commandInf);
                REQUEST_HANDLER_CASE(GET_PARM_DUAL_CAM_MODE,  getDualCamMode,    g.u.dualCamMode);
                REQUEST_HANDLER_CASE(GET_PARM_SUB_CAM_DATA,   getSubCamData,     g.u.subCamData);
                REQUEST_HANDLER_CASE(GET_PARM_FAIRLIGHT,      getFairLightMode,  g.u.fairLightMode);
                REQUEST_HANDLER_CASE(GET_PARM_ROTATION,       getRotationAngle,  g.u.rotation);
                REQUEST_HANDLER_CASE(GET_PARM_WATER_MARK,     getWaterMarkInfo,  g.u.watermark);
                REQUEST_HANDLER_CASE(GET_PARM_ZOOM_INF,       getZoomInf,        g.u.zoom);
                REQUEST_HANDLER_CASE(GET_PARM_SINGLE_BOKEH,   getSingleBokehInf, g.u.singleBokeh);
                REQUEST_HANDLER_CASE(GET_PARM_PLATFORM_PRIV,  getPlatformPriv,   g.u.platformPriv);
                REQUEST_HANDLER_DEFAULT(g.type, "get parm");
            }
        } break;
        case PARM_TYPE_SET: {
            switch (request->s.type) {
                REQUEST_HANDLER_CASE(SET_PARM_MULTI_SHOT,    setMultiShot,      s.u.multiShot);
                REQUEST_HANDLER_CASE(SET_PARM_AE_BRACKET,    setMultiExposure,  s.u.multiExposure);
                REQUEST_HANDLER_CASE(SET_PARM_PLATFORM_PRIV, setPlatformPrivSetting, s.u.platformPriv);
                REQUEST_HANDLER_DEFAULT(s.type, "set parm");
            }
        } break;
        case PARM_TYPE_OTHERS: {
            switch (request->o.type) {
                REQUEST_HANDLER_CASE(OTHER_TYPE_EVT_CALLBACK,  sendEvtCallback,  o.u.evt);
                REQUEST_HANDLER_CASE(OTHER_TYPE_DATA_CALLBACK, sendDataCallback, o.u.data);
                REQUEST_HANDLER_DEFAULT(o.type, "other operation");
            }
        } break;
        case PARM_TYPE_MAX_INVALID:
        REQUEST_HANDLER_DEFAULT(type, "operation type");
    }

    return rc;
}

// ------------ GET PARMS START ------------

#define DEFINE_GET_FUNCTIONS(CATEGORY, type, ele) \
    int32_t PlatformOpsIntf::get##CATEGORY(CATEGORY &result) \
    { \
        int32_t rc = NOT_INITED; \
    \
        RWLock::AutoRLock l(mParmLock); \
        if (mParmValid[type]) { \
            result = mGetParms[type].ele; \
            rc = NO_ERROR; \
        } \
    \
        return rc; \
    }

DEFINE_GET_FUNCTIONS(PreviewDim,     GET_PARM_PREVIEW_SIZE,  previewSize);
DEFINE_GET_FUNCTIONS(PictureDim,     GET_PARM_PICTURE_SIZE,  pictureSize);
DEFINE_GET_FUNCTIONS(VideoDim,       GET_PARM_VIDEO_SIZE,    videoSize);
DEFINE_GET_FUNCTIONS(FlashMode,      GET_PARM_FLASH_MODE,    flashMode);
DEFINE_GET_FUNCTIONS(HdrMode,        GET_PARM_HDR_MODE,      hdrMode);
DEFINE_GET_FUNCTIONS(LongshotMode,   GET_PARM_LONG_SHOT,     longShot);
DEFINE_GET_FUNCTIONS(LongExpMode,    GET_PARM_LONG_EXPOSURE, longExposure);
DEFINE_GET_FUNCTIONS(SmartshotMode,  GET_PARM_SMART_SHOT,    smartShot);
DEFINE_GET_FUNCTIONS(ImageStabMode,  GET_PARM_IMAGE_STAB,    imageStab);
DEFINE_GET_FUNCTIONS(VideoStabMode,  GET_PARM_VIDEO_STAB,    videoStab);
DEFINE_GET_FUNCTIONS(FlipMode,       GET_PARM_FLIP_MODE,     flipMode);
DEFINE_GET_FUNCTIONS(NightShotMode,  GET_PARM_NIGHT_SHOT,    nightShot);
DEFINE_GET_FUNCTIONS(BeautySetting,  GET_PARM_BEAUTY_FACE,   beautySetting);
DEFINE_GET_FUNCTIONS(FaceInfo,       GET_PARM_FACE_INFO,     faces);
DEFINE_GET_FUNCTIONS(AEInfo,         GET_PARM_AE_INFO,       ae);
DEFINE_GET_FUNCTIONS(AWBInfo,        GET_PARM_AWB_INFO,      awb);
DEFINE_GET_FUNCTIONS(AFInfo,         GET_PARM_AF_INFO,       af);
DEFINE_GET_FUNCTIONS(HdrInfo,        GET_PARM_HDR_INFO,      hdr);
DEFINE_GET_FUNCTIONS(SceneInfo,      GET_PARM_SCENE_INFO,    scene);
DEFINE_GET_FUNCTIONS(CameraTypeInf,  GET_PARM_CAM_TYPE,      camType);
DEFINE_GET_FUNCTIONS(CommandInf,     GET_PARM_COMMAND_INF,   commandInf);
DEFINE_GET_FUNCTIONS(DualCamMode,    GET_PARM_DUAL_CAM_MODE, dualCamMode);
DEFINE_GET_FUNCTIONS(SubCamData,     GET_PARM_SUB_CAM_DATA,  subCamData);
DEFINE_GET_FUNCTIONS(FairLightMode,  GET_PARM_FAIRLIGHT,     fairLightMode);
DEFINE_GET_FUNCTIONS(RotationAngle,  GET_PARM_ROTATION,      rotation);
DEFINE_GET_FUNCTIONS(WaterMarkInfo,  GET_PARM_WATER_MARK,    watermark);
DEFINE_GET_FUNCTIONS(ZoomInf,        GET_PARM_ZOOM_INF,      zoom);
DEFINE_GET_FUNCTIONS(SingleBokehInf, GET_PARM_SINGLE_BOKEH,  singleBokeh);
DEFINE_GET_FUNCTIONS(PlatformPriv,   GET_PARM_PLATFORM_PRIV, platformPriv);


// ------------ CONFIGS START ------------

#define CHECK_CONFIG_IMPL() \
    ({ \
        int32_t __rc = NO_ERROR; \
        if (ISNULL(mConfig)) { \
            LOGE(MODULE_PLATFORM_OPS, "Config interface not inited"); \
            __rc = NOT_INITED; \
        } \
        __rc; \
    })

#define DEFINE_GET_CONFIG_FUNC(func, type, call) \
    int32_t PlatformOpsIntf::func(type &result) \
    { \
        int32_t rc = CHECK_CONFIG_IMPL(); \
    \
        RWLock::AutoRLock l(mParmLock); \
        if (SUCCEED(rc)) { \
            mConfig->call(result); \
        } \
    \
        return rc; \
    }

};

