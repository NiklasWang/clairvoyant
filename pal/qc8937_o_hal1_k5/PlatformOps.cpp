#include "PlatformOps.h"
#include "Platform.h"
#include "CameraStatus.h"
#include "PlatformConfig.h"

#include "QCameraParameters.h"
#include "QCamera2HWI.h"

#undef LOGD
#undef LOGI
#undef LOGW
#undef LOGE
#undef CLOGx
#undef _LOG_H_
#include "Logs.h"

#define VISUAL_WIDTH  220
#define SCREEN_WIDTH  720
#define SEOUL_DP      40

namespace pandora {

using namespace qcamera;

PlatformOps::PlatformOps(QCamera2HardwareInterface *hwi) :
    mModule(MODULE_PLATFORM_OPS),
    mHWI(hwi)
{
    if (ISNULL(mHWI)) {
        LOGE(mModule, "Fatal: hwi is NULL");
    } else {
        init();
    }
}

void PlatformOps::init()
{
    if (NOTNULL(mHWI)) {
        mHWI->mRequireCtl.enable_bracket = false;
        mHWI->mRequireCtl.bracket_frame = INVALIDNUM;
        mHWI->mRequireCtl.frame_cnt = 0;
    }

    if (mHWI->mCameraId < 3) {
        strncpy(mSensorName, gCamCapability[mHWI->mCameraId]->sensor_name,
            SENSOR_NAME_MAX_LEN);
    }
    mParmValid[GET_PARM_CAM_TYPE] = false;
    if (NOTNULL(mHWI)) {
        if (mHWI->mCameraId == 0) {
            mGetParms[GET_PARM_CAM_TYPE].camType.type =
                CameraTypeInf::CAMERA_TYPE_BACK_0;
        } else if (mHWI->mCameraId == 1) {
            mGetParms[GET_PARM_CAM_TYPE].camType.type =
                CameraTypeInf::CAMERA_TYPE_FRONT_0;
        }
        mParmValid[GET_PARM_CAM_TYPE] = true;
    }

    {
        char tmp[SYSTEM_PROP_MAX_LEN];
        mProductName[0] = '\0';
        property_get("ro.zuk.product.market", mProductName, "Unknown Product");
        mPackageName[0] = '\0';
        property_get("vendor.camera.packagename", mPackageName, "Unknown App");

        tmp[0] = '\0';
        property_get("ro.vendor.build.date.utc", tmp, "1521035655");
        time_t utcTime = atoi(tmp);
        ctime_r(&utcTime, mBuildVersion);
        tmp[0] = '\0';
        property_get("ro.sf.lcd_density", tmp, "480");
        mDeviceDpi = atoi(tmp);
        tmp[0] = '\0';
        property_get("ro.build.fingerprint", tmp, "NA");
        char *pos = strstr(tmp, "userdebug");
        mDebugBuild = NOTNULL(pos);
    }

    mLastPlatformPrivSet.denoise = 0;
    mLastPlatformPrivSet.sharpen = 0;

    mParmValid[GET_PARM_COMMAND_INF] = false;
    mGetParms[GET_PARM_COMMAND_INF].commandInf.type =
        CommandInf::COMMAND_TYPE_LONGSHOT_OFF;
    mParmValid[GET_PARM_COMMAND_INF] = true;
}

bool PlatformOps::isInited()
{
    return NOTNULL(mHWI);
}

const char *PlatformOps::whoamI()
{
    return "K5 Qcom8937 Android O(8.0) Hal v1";
}

sp<ConfigInterface> &PlatformOps::getConfig()
{
    if (ISNULL(mConfig)) {
        uint32_t camId = 0;
        if (mGetParms[GET_PARM_CAM_TYPE].camType.type ==
            CameraTypeInf::CAMERA_TYPE_FRONT_0) {
            camId = 1;
        } else if (mGetParms[GET_PARM_CAM_TYPE].camType.type ==
                CameraTypeInf::CAMERA_TYPE_MONO_0) {
            camId = 2;
        } else {
            camId = 0;
        }
        mConfig = new Config(camId);
    }
    return mConfig;
}

#define CHECK_PLATFORM_IMPL() \
    ({ \
        int32_t __rc = NO_ERROR; \
        if (ISNULL(mHWI)) { \
            LOGE(mModule, "Platform not inited"); \
            __rc = NOT_INITED; \
        } \
        __rc; \
    })

// ------------ SET PARMS START ------------
int32_t PlatformOps::setMultiShot(MultiShotParm &parm)
{
    int32_t rc = CHECK_PLATFORM_IMPL();

    if (SUCCEED(rc)) {
       mHWI->mParameters.setNumOfSnapshot(parm.num);
       //mHWI->mDualCamShareInfo.mNumOfSnapshot= parm.num;
        if (parm.num) {
            mHWI->mRequireCtl.enable_bracket = true;
            mHWI->mRequireCtl.bracket_frame = parm.num;
            mHWI->mRequireCtl.frame_cnt = 0;
        } else {
            mHWI->mRequireCtl.enable_bracket = false;
            mHWI->mRequireCtl.bracket_frame = INVALIDNUM;
            mHWI->mRequireCtl.frame_cnt = 0;
        }
    }

    return rc;
}

int32_t PlatformOps::setMultiExposure(MultiExporeParm &parm)
{
    int32_t rc = CHECK_PLATFORM_IMPL();

    char str[MAX_EXP_BRACKETING_LENGTH] = {0};
    char value[20];
    const char *switchStr = NULL;

    for (uint32_t i = 0; i < parm.size(); i++) {
        memset(value, 0, sizeof(value));
        snprintf(value, sizeof(value), "%d", parm.get(i));
        if (i != 0) {
            strcat(str, ",");
        }
        strcat(str, value);
    }

    switchStr = (parm.size() > 1) ?
        QCameraParameters::AE_BRACKET : QCameraParameters::AE_BRACKET_OFF;
    Mutex::Autolock lock(mHWI->mParameters.mLock);
    //mHWI->mParameters()->set(QCameraParameters::KEY_QC_CAPTURE_BURST_EXPOSURE, str);
    mHWI->mParameters()->setCapBurstExp(str);
    mHWI->mParameters()->initBatchUpdate(mHWI->mParameters()->m_pParamBuf);
    mHWI->mParameters()->setAEBracket(switchStr);
    mHWI->mParameters()->commitParameters();
    mHWI->mParameters()->setNumOfSnapshot();

    if (parm.size()) {
        mHWI->mRequireCtl.enable_bracket = true;
        mHWI->mRequireCtl.bracket_frame = parm.size();
        mHWI->mRequireCtl.frame_cnt = 0;
    } else {
        mHWI->mRequireCtl.enable_bracket = false;
        mHWI->mRequireCtl.bracket_frame = INVALIDNUM;
        mHWI->mRequireCtl.frame_cnt = 0;
    }

    return rc;
}

int32_t PlatformOps::setPlatformPrivSetting(PlatformPrivSet &parm)
{
    int32_t rc = NO_ERROR;

    return rc;
}

// ------------ OTHER TYPES START ------------
int32_t PlatformOps::sendEvtCallback(EvtCallbackInf &evt)
{
    int32_t rc = CHECK_PLATFORM_IMPL();

    if (SUCCEED(rc)) {
        mHWI->sendEvtNotify(evt.evt, evt.type, evt.arg);
    }

    return rc;
}

int32_t PlatformOps::sendDataCallback(DataCallbackInf &task)
{
    int32_t rc = ISNULL(mHWI) ? NOT_INITED : NO_ERROR;
    size_t bufSize = 0;
    camera_memory_t *buf = NULL;

    LOGD(mModule, "data type %d %d %p", task.type, task.size, task.data);

    if (SUCCEED(rc)) {
        bufSize = 1 *sizeof(int32_t) //data type
                + 1 *sizeof(size_t)  //data len
                + task.size;         //data
        buf = mHWI->mGetMemory(
            INVALIDNUM, bufSize, 1, mHWI->mCallbackCookie);
        if (ISNULL(buf)) {
            LOGE(mModule, "Not enough memory for %d bytes data callback",
                bufSize);
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        int32_t *cbDat = (int32_t *)buf->data;
        cbDat[0] = task.type;
        cbDat[1] = task.size;
        memcpy(cbDat + 2, task.data, task.size);
    }

    if (SUCCEED(rc) || !SUCCEED(rc)) {
        if (NOTNULL(task.release)) {
            task.release(task.data);
        }
    }

    if (SUCCEED(rc)) {
        qcamera_callback_argm_t cbArg;
        memset(&cbArg, 0, sizeof(qcamera_callback_argm_t));
        cbArg.cb_type = QCAMERA_DATA_CALLBACK;
        cbArg.msg_type = CAMERA_MSG_META_DATA;
        cbArg.data = buf;
        cbArg.user_data = buf;
        cbArg.cookie = mHWI;
        cbArg.release_cb = mHWI->releaseCameraMemory;
        rc = mHWI->m_cbNotifier.notifyCallback(cbArg);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "fail sending data notification");
            buf->release(buf);
        }
    }

    return rc;
}

// ------------ CAMERA STATUS INFO -----------
#define DEBUG_METADATA_PARMS 0

AWBInfo &operator<<(AWBInfo &lhs, const cam_awb_params_t &dat)
{
    if (DEBUG_METADATA_PARMS) {
        LOGI(MODULE_PLATFORM_OPS,
            "cct_value %d rgain %.4f ggain %.4f bgain %.4f",
            dat.cct_value, dat.rgb_gains.r_gain,
            dat.rgb_gains.g_gain, dat.rgb_gains.b_gain);
    }

    lhs.color_temp = dat.cct_value;
    lhs.rgain = dat.rgb_gains.r_gain;
    lhs.ggain = dat.rgb_gains.g_gain;
    lhs.bgain = dat.rgb_gains.b_gain;

    return lhs;
}

AEInfo &operator<<(AEInfo &lhs, const cam_3a_params_t &dat)
{
    if (DEBUG_METADATA_PARMS) {
        LOGI(MODULE_PLATFORM_OPS,
            "cam_metadata_info_t exp_time %.4f, "
            "real_gain %.4f iso_value %d flash_needed %d "
            "lux index %f luma target %d",
            dat.exp_time, dat.iso_value * 1.0 / 100,
            dat.iso_value, dat.flash_needed,
            dat.lux_idx, dat.est_snap_target);
    }

    lhs.exp_time = dat.exp_time;
    lhs.real_gain = dat.iso_value * 1.0 / 100;
    lhs.iso_value = dat.iso_value;
    lhs.flash_needed = dat.flash_needed;
    lhs.lux_index = (int32_t)dat.lux_idx;
    lhs.luma_target = dat.est_snap_target;

    return lhs;
}

AFInfo &operator<<(AFInfo &lhs, const cam_auto_focus_data_t &dat)
{
    if (DEBUG_METADATA_PARMS) {
        LOGI(MODULE_PLATFORM_OPS,
            "cam_metadata_info_t focus_pos %d, focus_state %d",
            dat.focus_pos, dat.focus_state);
    }

    lhs.curr_pos = dat.focus_pos;
    lhs.total_pos = dat.focus_dist
        .focus_distance[CAM_FOCUS_DISTANCE_FAR_INDEX];
    lhs.start_pos = dat.focus_dist
        .focus_distance[CAM_FOCUS_DISTANCE_NEAR_INDEX];
    // Warning, state enum should same as qualcomm one
    lhs.state = static_cast<AFInfo::AfStatus>(dat.focus_state);


    return lhs;
}

HdrInfo &operator<<(HdrInfo &lhs, const cam_asd_hdr_scene_data_t &dat)
{
    if (DEBUG_METADATA_PARMS) {
        LOGI(MODULE_PLATFORM_OPS,
            "cam_metadata_info_t is_hdr_scene %d, hdr_confidence %.2f",
            dat.is_hdr_scene, dat.hdr_confidence);
    }

    lhs.is_hdr_scene = dat.is_hdr_scene;
    lhs.hdr_confidence = dat.hdr_confidence;

    return lhs;
}

SceneInfo &operator<<(SceneInfo &lhs, const cam_asd_decision_t &dat)
{
    if (DEBUG_METADATA_PARMS) {
        LOGI(MODULE_PLATFORM_OPS,
            "cam_metadata_info_t scene %d", dat.detected_scene);
    }

    lhs.type = static_cast<SceneInfo::SceneType>(dat.detected_scene);

    return lhs;
}

FaceInfo &operator<<(FaceInfo &lhs, const cam_face_detection_data_t &dat)
{
    lhs.clear();

    for (int32_t i = 0; i < dat.num_faces_detected; i++) {
        FaceInfo::Face face = {
            .rect = {
                .left = dat.faces[i].face_boundary.left,
                .top = dat.faces[i].face_boundary.top,
                .width = dat.faces[i].face_boundary.width,
                .height = dat.faces[i].face_boundary.height,
            },
            .id = dat.faces[i].face_id,
            .score = dat.faces[i].score,
            .frameSize = {
                .w = dat.fd_frame_dim.width,
                .h = dat.fd_frame_dim.height,
            },
            .frameType = FRAME_TYPE_PREVIEW,
        };

        lhs.add(face);
    }

    return lhs;
}

#define IF_CMETA_AVAILABLE(META_TYPE, META_PTR_NAME, META_ID, TABLE_PTR) \
        const META_TYPE *META_PTR_NAME = \
        (((NULL != TABLE_PTR) && (TABLE_PTR->is_valid[META_ID])) ? \
            (&TABLE_PTR->data.member_variable_##META_ID[ 0 ]) : \
            (NULL)); \
        if (NULL != META_PTR_NAME) \


int32_t PlatformOps::onMetadataAvailable(const void *dat)
{
    int32_t rc = NO_ERROR;

    const metadata_buffer_t *metadata =
        static_cast<const metadata_buffer_t *>(dat);

    RWLock::AutoWLock l(mParmLock);

    mParmValid[GET_PARM_FACE_INFO] = false;
    IF_CMETA_AVAILABLE(cam_face_detection_data_t, faces_data,
        CAM_INTF_META_FACE_DETECTION, metadata) {
        mGetParms[GET_PARM_FACE_INFO].faces << *faces_data;
        mParmValid[GET_PARM_FACE_INFO] = true;
    }

    mParmValid[GET_PARM_AE_INFO] = false;
    IF_CMETA_AVAILABLE(cam_3a_params_t, q3a_data,
        CAM_INTF_META_AEC_INFO, metadata) {
        mGetParms[GET_PARM_AE_INFO].ae << *q3a_data;
        mParmValid[GET_PARM_AE_INFO] = true;
    }

    mParmValid[GET_PARM_AWB_INFO] = false;
    IF_CMETA_AVAILABLE(cam_awb_params_t, awb_data,
        CAM_INTF_META_AWB_INFO, metadata) {
        mGetParms[GET_PARM_AWB_INFO].awb << *awb_data;
        mParmValid[GET_PARM_AWB_INFO] = true;
    }
#if 0
#if 0
    mParmValid[GET_PARM_HDR_INFO] = false;
    IF_CMETA_AVAILABLE(cam_asd_hdr_scene_data_t, hdr_data,
        CAM_INTF_META_ASD_HDR_SCENE_DATA, metadata) {
        mGetParms[GET_PARM_HDR_INFO].hdr << *hdr_data;
        mParmValid[GET_PARM_HDR_INFO] = true;
    }
#else
    mParmValid[GET_PARM_HDR_INFO] = false;
    if (mHWI->mParameters()->isHDREnabled()) {
        mGetParms[GET_PARM_HDR_INFO].hdr.is_hdr_scene = 1;
        mGetParms[GET_PARM_HDR_INFO].hdr.hdr_confidence = 1.0f;
    } else {
        mGetParms[GET_PARM_HDR_INFO].hdr.is_hdr_scene = 0;
        mGetParms[GET_PARM_HDR_INFO].hdr.hdr_confidence = 0.0f;
    }
    mParmValid[GET_PARM_HDR_INFO] = true;
#endif
#endif
    mParmValid[GET_PARM_SCENE_INFO] = false;
    IF_CMETA_AVAILABLE(cam_asd_decision_t, asd_data,
        CAM_INTF_META_ASD_SCENE_INFO, metadata) {
        mGetParms[GET_PARM_SCENE_INFO].scene << *asd_data;
        mParmValid[GET_PARM_SCENE_INFO] = true;
    }

    {
        mParmValid[GET_PARM_AF_INFO] = false;
        cam_auto_focus_data_t focus;
        IF_CMETA_AVAILABLE(uint32_t, afState,
            CAM_INTF_META_AF_STATE, metadata) {
            focus.focus_state = (cam_af_state_t)(*afState);
        }
        IF_CMETA_AVAILABLE(uint32_t, focusMode,
            CAM_INTF_PARM_FOCUS_MODE, metadata) {
             focus.focus_mode = (cam_focus_mode_type)(*focusMode);
        }
        IF_CMETA_AVAILABLE(float, focusDistance,
            CAM_INTF_META_LENS_FOCUS_DISTANCE, metadata) {
            focus.focus_dist.focus_distance[
                CAM_FOCUS_DISTANCE_OPTIMAL_INDEX] = *focusDistance;
        }
        IF_CMETA_AVAILABLE(float, focusRange,
            CAM_INTF_META_LENS_FOCUS_RANGE, metadata) {
            focus.focus_dist.focus_distance[
                CAM_FOCUS_DISTANCE_NEAR_INDEX] = focusRange[0];
            focus.focus_dist.focus_distance[
                CAM_FOCUS_DISTANCE_FAR_INDEX] = focusRange[1];
        }
        mGetParms[GET_PARM_AF_INFO].af << focus;
        mParmValid[GET_PARM_AF_INFO] = true;
    }

    {
        mParmValid[GET_PARM_SUB_CAM_DATA] = false;
        if (metadata->is_tuning_params_valid) {
            QCamera2HardwareInterface::sensor_tuning_bet_metadata_t * bet_metadata =
                (QCamera2HardwareInterface::sensor_tuning_bet_metadata_t *)(
                    (uint8_t *)&metadata->tuning_params.data[TUNING_SENSOR_DATA_OFFSET]);
            int16_t *p_dac_value = (int16_t *)&(bet_metadata->dac_value);
            if (mHWI->mCameraId == 0) {
                int16_t dac_value = *p_dac_value;
                mGetParms[GET_PARM_SUB_CAM_DATA].subCamData.mainVCMDAC =
                    dac_value > 0 ? dac_value : 1024 + dac_value;

                dac_value = mHWI->mDualCamShareInfo.subVCMDAC;
                mGetParms[GET_PARM_SUB_CAM_DATA].subCamData.subVCMDAC =
                    dac_value > 0  ? dac_value : 1024 + dac_value;
            }

            IF_CMETA_AVAILABLE(cam_stream_crop_info_t, cppCropInfo,
                CAM_INTF_META_SNAP_CROP_INFO_CPP, metadata) {
                mGetParms[GET_PARM_SUB_CAM_DATA].subCamData.roiMapW =
                    cppCropInfo->roi_map.width;
                mGetParms[GET_PARM_SUB_CAM_DATA].subCamData.roiMapH =
                    cppCropInfo->roi_map.height;
            }

            mParmValid[GET_PARM_SUB_CAM_DATA] = true;
        }
    }
    return NO_ERROR;
}

int32_t PlatformOps::onParameterAvailable(const void *data)
{
    int32_t rc = NO_ERROR;

    const QCameraParameters *parm =
        static_cast<const QCameraParameters *>(data);

    RWLock::AutoWLock l(mParmLock);

    if (SUCCEED(rc)) {
        int32_t w = 0, h = 0;

        mParmValid[GET_PARM_PREVIEW_SIZE] = false;
        parm->getPreviewSize(&w, &h);
        if (w > 0 && h > 0) {
            mGetParms[GET_PARM_PREVIEW_SIZE].previewSize.w = w;
            mGetParms[GET_PARM_PREVIEW_SIZE].previewSize.h = h;
            mParmValid[GET_PARM_PREVIEW_SIZE] = true;
        }

        mParmValid[GET_PARM_PICTURE_SIZE] = false;
        parm->getPictureSize(&w, &h);
        if (w > 0 && h > 0) {
            mGetParms[GET_PARM_PICTURE_SIZE].pictureSize.w = w;
            mGetParms[GET_PARM_PICTURE_SIZE].pictureSize.h = h;
            mParmValid[GET_PARM_PICTURE_SIZE] = true;
        }

        mParmValid[GET_PARM_PLATFORM_PRIV] = false;
#if 0
        int width = 0, height = 0;
        mHWI->getPicSizeFromAPP(width, height);
        mHWI->mParameters.setJpegRotation(mHWI->mParameters.getRotation());
        if (((90 == mHWI->mParameters.getJpegRotation())
                || (270 == mHWI->mParameters.getJpegRotation()))
                && (mHWI->needRotationReprocess())) {
            mGetParms[GET_PARM_PLATFORM_PRIV].platformPriv.w = (int32_t)height;
            mGetParms[GET_PARM_PLATFORM_PRIV].platformPriv.h = (int32_t)width;
        } else {
            mGetParms[GET_PARM_PLATFORM_PRIV].platformPriv.w = (int32_t)width;
            mGetParms[GET_PARM_PLATFORM_PRIV].platformPriv.h = (int32_t)height;
        }
        mParmValid[GET_PARM_PLATFORM_PRIV] = true;
#endif

        mParmValid[GET_PARM_VIDEO_SIZE] = false;
        parm->getVideoSize(&w, &h);
        if (mHWI->mParameters.getRecordingHintValue() == 0) {
            w = h = 0;
        }
        if (w > 0 && h > 0) {
            mGetParms[GET_PARM_VIDEO_SIZE].videoSize.w = w;
            mGetParms[GET_PARM_VIDEO_SIZE].videoSize.h = h;
            mParmValid[GET_PARM_VIDEO_SIZE] = true;
        }
        if (!(w > 0 && h > 0) && mHWI->mParameters.getRecordingHintValue()) {
            parm->getPreviewSize(&w, &h);
            mGetParms[GET_PARM_VIDEO_SIZE].videoSize.w = w;
            mGetParms[GET_PARM_VIDEO_SIZE].videoSize.h = h;
            mParmValid[GET_PARM_VIDEO_SIZE] = true;
        }
    }

    if (SUCCEED(rc)) {
        mParmValid[GET_PARM_BEAUTY_FACE] = false;
        const char *val = parm->get("mot-beauty");
        if (NOTNULL(val)) {
            if (!strcmp(val, "auto")) {
                mGetParms[GET_PARM_BEAUTY_FACE].beautySetting.mode =
                    BEAUTY_MODE_ON;
                mGetParms[GET_PARM_BEAUTY_FACE].beautySetting.arg = 0;
            } else if (!strcmp(val, "manual")) {
                mGetParms[GET_PARM_BEAUTY_FACE].beautySetting.mode =
                    BEAUTY_MODE_MANUAL;
                const char *val2 = parm->get("mot-beauty-level");
                mGetParms[GET_PARM_BEAUTY_FACE].beautySetting.arg =
                    NOTNULL(val2)? atoi(val2) : 0;
            } else {
                mGetParms[GET_PARM_BEAUTY_FACE].beautySetting.mode =
                    BEAUTY_MODE_OFF;
                mGetParms[GET_PARM_BEAUTY_FACE].beautySetting.arg = 0;
            }
            mParmValid[GET_PARM_BEAUTY_FACE] = true;
        }
    }

    if (SUCCEED(rc)) {
        mParmValid[GET_PARM_HDR_MODE] = false;
        const char *val = parm->get(QCameraParameters::KEY_SCENE_MODE);
        if (NOTNULL(val)) {
            if (!strcmp(val, QCameraParameters::SCENE_MODE_HDR)) {
                mGetParms[GET_PARM_HDR_MODE].hdrMode = HDR_MODE_FORCED_ON;
            } else if (!strcmp(val, QCameraParameters::SCENE_MODE_ASD)) {
                mGetParms[GET_PARM_HDR_MODE].hdrMode = HDR_MODE_AUTO;
            } else {
                mGetParms[GET_PARM_HDR_MODE].hdrMode = HDR_MODE_FORCED_OFF;
            }
            mParmValid[GET_PARM_HDR_MODE] = true;
        }
    }

    if (SUCCEED(rc)) {
        mParmValid[GET_PARM_FLASH_MODE] = false;
        const char *val = parm->get(QCameraParameters::KEY_FLASH_MODE);
        if (NOTNULL(val)) {
            FlashMode curr;
            if (!strcmp(val, QCameraParameters::FLASH_MODE_ON)) {
                mGetParms[GET_PARM_FLASH_MODE].flashMode = FLASH_MODE_FORCED_ON;
            } else if (!strcmp(val, QCameraParameters::FLASH_MODE_AUTO)) {
                mGetParms[GET_PARM_FLASH_MODE].flashMode = FLASH_MODE_AUTO;
            } else {
                mGetParms[GET_PARM_FLASH_MODE].flashMode = FLASH_MODE_FORCED_OFF;
            }
            mParmValid[GET_PARM_FLASH_MODE] = true;
        }
    }

    if (SUCCEED(rc)) {
        mParmValid[GET_PARM_LONG_SHOT] = false;
        const char *val = parm->get("longshot");
        if (NOTNULL(val)) {
            int32_t on = !strcmp(val, "on");
            mGetParms[GET_PARM_LONG_SHOT].longShot =
                on ? LONG_SHOT_MODE_ON : LONG_SHOT_MODE_OFF;
            mParmValid[GET_PARM_LONG_SHOT] = true;
        }
    }

    if (SUCCEED(rc)) {
        mParmValid[GET_PARM_SMART_SHOT] = false;
        const char *val = parm->get("smart-shot");
        if (NOTNULL(val)) {
            int32_t on = !strcmp(val, "on");
            mGetParms[GET_PARM_SMART_SHOT].smartShot =
                on ? SMART_SHOT_MODE_ON : SMART_SHOT_MODE_OFF;
            mParmValid[GET_PARM_SMART_SHOT] = true;
        }
    }

    if(SUCCEED(rc)){
        mParmValid[GET_PARM_VIDEO_STAB] = false;
        const char *val = parm->get("movie-solid");
        if (NOTNULL(val)) {
            int32_t on = !strcmp(val, "on");
            mGetParms[GET_PARM_VIDEO_STAB].videoStab =
                on ? VIDEO_STAB_MODE_VIDEO_ONLY : VIDEO_STAB_MODE_OFF;
            mParmValid[GET_PARM_VIDEO_STAB] = true;
        }
    }

    if(SUCCEED(rc)){
        mParmValid[GET_PARM_FLIP_MODE] = false;
        const char *val = parm->get("video-flip");
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
        const char *val = parm->get(QCameraParameters::KEY_SCENE_MODE);
        if(NOTNULL(val)){
            int32_t on = !strcmp(val, QCameraParameters::SCENE_MODE_NIGHT) ||
                !strcmp(val, QCameraParameters::SCENE_MODE_NIGHT_PORTRAIT);
            mGetParms[GET_PARM_IMAGE_STAB].imageStab =
                on ? IMAGE_STAB_MODE_ON : IMAGE_STAB_MODE_OFF;
            mParmValid[GET_PARM_IMAGE_STAB] = true;
        }
    }

    if (SUCCEED(rc)) {
        mParmValid[GET_PARM_NIGHT_SHOT] = false;
        const char *val = parm->get("night-shot");
        if (NOTNULL(val)) {
            FlashMode curr;
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
        const char *val = parm->get("long-exposure");
        if(NOTNULL(val)){
            int32_t on = !strcmp(val, "on");
            mGetParms[GET_PARM_LONG_EXPOSURE].longExposure.on = on;
            mGetParms[GET_PARM_LONG_EXPOSURE].longExposure.time =
                on ? 0.0f : 0.0f; // TODO: impl long exposure here if necessary
            mParmValid[GET_PARM_LONG_EXPOSURE] = true;
        }
    }

    if (SUCCEED(rc)) {
        mParmValid[GET_PARM_SUB_CAM_DATA] = false;
        const char *val = parm->get("blur-level");
        if (NOTNULL(val)){
            mGetParms[GET_PARM_SUB_CAM_DATA].subCamData.blurLevel= atoi(val);
        } else {
            mGetParms[GET_PARM_SUB_CAM_DATA].subCamData.blurLevel= 5;
        }
        val = parm->get(QCameraParameters::KEY_FOCUS_AREAS);
        if (NOTNULL(val)) {
            cam_roi_info_t af_roi_value;
            extern int32_t getFocusArea(const char *focusAreasStr,
                CamDimension streamSize, cam_roi_info_t &af_roi_value);
            getFocusArea(val, mGetParms[GET_PARM_PICTURE_SIZE].pictureSize, af_roi_value);
            mGetParms[GET_PARM_SUB_CAM_DATA].subCamData.focusX =
                af_roi_value.roi[0].left + af_roi_value.roi[0].width / 2;
            mGetParms[GET_PARM_SUB_CAM_DATA].subCamData.focusY =
                af_roi_value.roi[0].top + af_roi_value.roi[0].height / 2;

            //get preview focus area
            getFocusArea(val, mGetParms[GET_PARM_PREVIEW_SIZE].previewSize, af_roi_value);
            mGetParms[GET_PARM_SUB_CAM_DATA].subCamData.previewFocusX =
                af_roi_value.roi[0].left + af_roi_value.roi[0].width / 2;
            mGetParms[GET_PARM_SUB_CAM_DATA].subCamData.previewFocusY =
                af_roi_value.roi[0].top + af_roi_value.roi[0].height / 2;

        } else {
            mGetParms[GET_PARM_SUB_CAM_DATA].subCamData.focusX =
                mGetParms[GET_PARM_PICTURE_SIZE].pictureSize.w / 2;
            mGetParms[GET_PARM_SUB_CAM_DATA].subCamData.focusY =
                mGetParms[GET_PARM_PICTURE_SIZE].pictureSize.h / 2;

            //set default preview focus area
            mGetParms[GET_PARM_SUB_CAM_DATA].subCamData.previewFocusX =
                mGetParms[GET_PARM_PICTURE_SIZE].previewSize.w / 2;
            mGetParms[GET_PARM_SUB_CAM_DATA].subCamData.previewFocusY =
                mGetParms[GET_PARM_PICTURE_SIZE].previewSize.h / 2;

        }
        mParmValid[GET_PARM_SUB_CAM_DATA] = true;
    }

    if (SUCCEED(rc)) {
        mParmValid[GET_PARM_SUB_CAM_DATA] = false;
        if ((gCamCapability[0] != NULL) && (gCamCapability[0]->otp_valid == 1)) {
            mGetParms[GET_PARM_SUB_CAM_DATA].subCamData.mainOTP =
                gCamCapability[0]->otp_data;
            mGetParms[GET_PARM_SUB_CAM_DATA].subCamData.mainOTPSize =
                gCamCapability[0]->otp_size;
            mGetParms[GET_PARM_SUB_CAM_DATA].subCamData.mainModuleName =
                gCamCapability[0]->sensor_name;
        }

        if ((gCamCapability[2] != NULL) && (gCamCapability[2]->otp_valid == 1)) {
            mGetParms[GET_PARM_SUB_CAM_DATA].subCamData.subOTP =
                gCamCapability[2]->otp_data;
            mGetParms[GET_PARM_SUB_CAM_DATA].subCamData.subOTPSize =
                gCamCapability[2]->otp_size;
            mGetParms[GET_PARM_SUB_CAM_DATA].subCamData.subModuleName =
                gCamCapability[2]->sensor_name;
        }

        if (gCamCapability[1] != NULL) {
            mGetParms[GET_PARM_SUB_CAM_DATA].subCamData.frontModuleName =
                gCamCapability[1]->sensor_name;
        }

        mParmValid[GET_PARM_SUB_CAM_DATA] = true;
    }

    if (SUCCEED(rc)) {
        mParmValid[GET_PARM_DUAL_CAM_MODE] = false;
        mGetParms[GET_PARM_DUAL_CAM_MODE].dualCamMode = DUAL_CAMERA_NONE;
        const char *val = parm->get(QCameraParameters::KEY_AL_BOKEH_ENABLE);
        if(NOTNULL(val)){
            int32_t on = !strcmp(val, QCameraParameters::VALUE_ON);
            if (on) {
                mGetParms[GET_PARM_DUAL_CAM_MODE].dualCamMode = DUAL_CAMERA_MODE_ONLY_FOR_K5;
            }
            mParmValid[GET_PARM_DUAL_CAM_MODE] = true;
        }
    }

    if (SUCCEED(rc)) {
        mParmValid[GET_PARM_AE_LOCK_STATE] = false;
        const char *val = parm->get(QCameraParameters::KEY_AUTO_EXPOSURE_LOCK);
        if(NOTNULL(val)){
            int32_t on = !strcmp(val, QCameraParameters::VALUE_TRUE);
            if (on) {
                mGetParms[GET_PARM_AE_LOCK_STATE].aeLockState = AE_LOCK_ON;
            }
            mParmValid[GET_PARM_AE_LOCK_STATE] = true;
        }

    }

	if (SUCCEED(rc)) {
        mParmValid[GET_PARM_ROTATION] = false;
        int32_t rotation = parm->getInt(QCameraParameters::KEY_ROTATION);
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
        mParmValid[GET_PARM_WATER_MARK] = false;
        const char *val = parm->get("lenovo-water-mark");
        if(NOTNULL(val)) {
            int32_t on = !strcmp(val, QCameraParameters::VALUE_ON);
            if (on) {
                mGetParms[GET_PARM_WATER_MARK].watermark.mode = WATERMARK_MODE_ON;
                mGetParms[GET_PARM_WATER_MARK].watermark.dp   = SEOUL_DP;
                mGetParms[GET_PARM_WATER_MARK].watermark.dpi  = mDeviceDpi;
                mGetParms[GET_PARM_WATER_MARK].watermark.wmRatio.numerator   = VISUAL_WIDTH;
                mGetParms[GET_PARM_WATER_MARK].watermark.wmRatio.denominator = SCREEN_WIDTH;
                mGetParms[GET_PARM_WATER_MARK].watermark.product = mProductName;
                mGetParms[GET_PARM_WATER_MARK].watermark.build   = mBuildVersion;
                mGetParms[GET_PARM_WATER_MARK].watermark.package = mPackageName;
                mGetParms[GET_PARM_WATER_MARK].watermark.debug   = mDebugBuild;
                mGetParms[GET_PARM_WATER_MARK].watermark.id      = 1;
                mGetParms[GET_PARM_WATER_MARK].watermark.enableLogo  = false;
                mGetParms[GET_PARM_WATER_MARK].watermark.enableTexts = false;
            } else {
                mGetParms[GET_PARM_WATER_MARK].watermark.mode = WATERMARK_MODE_OFF;
            }
        }
        mParmValid[GET_PARM_WATER_MARK] = true;
    }

    return NO_ERROR;
}

int32_t PlatformOps::onCommandAvailable(int32_t cmd, int32_t arg1, int32_t arg2)
{
    int32_t rc = NO_ERROR;

    RWLock::AutoWLock l(mParmLock);
    mParmValid[GET_PARM_COMMAND_INF] = false;
    if(cmd == CAMERA_CMD_LONGSHOT_ON) {
        mGetParms[GET_PARM_COMMAND_INF].commandInf.type =
            CommandInf::COMMAND_TYPE_LONGSHOT_ON;
    } else if(cmd == CAMERA_CMD_LONGSHOT_OFF) {
        mGetParms[GET_PARM_COMMAND_INF].commandInf.type =
            CommandInf::COMMAND_TYPE_LONGSHOT_OFF;
    }
    mParmValid[GET_PARM_COMMAND_INF] = true;
    return rc;
}

int32_t parseCameraAreaString(const char *str,
    int max_num_areas, cam_area_t *pAreas, int& num_areas_found)
{
    char area_str[32];
    const char *start, *end, *p;
    start = str; end = NULL;
    int values[5], index=0;
    num_areas_found = 0;

    memset(values, 0, sizeof(values));
    while(start != NULL) {
        if(*start != '(') {
            LOGE(MODULE_PLATFORM_OPS, "error: Ill formatted area string: %s", str);
            return -1;
        }
        end = strchr(start, ')');
        if(end == NULL) {
            LOGE(MODULE_PLATFORM_OPS, "error: Ill formatted area string: %s", str);
            return -1;
        }
        int i;
        for (i=0,p=start; p<=end; p++, i++) {
           area_str[i] = *p;
        }
        area_str[i] = '\0';

        {
            int N = 5;
            char delim = ',';
            char *str = (char *)area_str;
            int *num = (int *) values;
            char *start, *end;
            //check if string starts and ends with parantheses
            if(str[0] != '(' || str[strlen(str)-1] != ')') {
                LOGE(MODULE_PLATFORM_OPS, "Invalid format of string %s, "
                    "valid format is (n1, n2, n3, n4 ...)", str);
                return -1;
            }
            start = (char*) area_str;
            start++;
            for(int i=0; i<N; i++) {
                *(num+i) = (int) strtol(start, &end, 10);
                if(*end != delim && i < N-1) {
                    LOGE(MODULE_PLATFORM_OPS,"Cannot find delimeter '%c' "
                        "in string \"%s\". end = %c", delim, str, *end);
                    return -1;
                }
                start = end+1;
            }
        }

        // no more areas than max_num_areas are accepted.
        if(index >= max_num_areas) {
            LOGE(MODULE_PLATFORM_OPS,"error: too many areas specified %s", str);
            return -1;
        }
        pAreas[index].rect.left = values[0];
        pAreas[index].rect.top = values[1];
        pAreas[index].rect.width = values[2] - values[0];
        pAreas[index].rect.height = values[3] - values[1];
        pAreas[index].weight = values[4];

        index++;
        start = strchr(end, '('); // serach for next '('
    }

    num_areas_found = index;

    return 0;
}

int32_t getFocusArea(const char *focusAreasStr,
    CamDimension streamSize, cam_roi_info_t &af_roi_value)
{
    #define MAX_NUM_FOCUS_AREAS  (5)
    cam_area_t *areas = (cam_area_t *)malloc(sizeof(cam_area_t) * MAX_NUM_FOCUS_AREAS);

    if (NULL == areas) {
        LOGE(MODULE_PLATFORM_OPS, "No memory for areas");
        return NO_MEMORY;
    }

    memset(areas, 0, sizeof(cam_area_t) * MAX_NUM_FOCUS_AREAS);
    int num_areas_found = 0;
    if (parseCameraAreaString(focusAreasStr,
        MAX_NUM_FOCUS_AREAS, areas, num_areas_found) != NO_ERROR) {
        LOGE(MODULE_PLATFORM_OPS, "Failed to parse the string: %s", focusAreasStr);
        free(areas);
        return -1;
    }

    //for special area string (0, 0, 0, 0, 0), set ROI to center
    if (num_areas_found == 1 &&
        areas[0].rect.left == 0 &&
        areas[0].rect.top == 0 &&
        areas[0].rect.width == 0 &&
        areas[0].rect.height == 0 &&
        areas[0].weight == 0) {
        areas[0].rect.left = -150;
        areas[0].rect.top = -200;
        areas[0].rect.width = 300;
        areas[0].rect.height = 400;
        areas[0].weight = 1;
    }

    memset(&af_roi_value, 0, sizeof(cam_roi_info_t));
    af_roi_value.num_roi = (uint8_t)num_areas_found;
    for (int i = 0; i < num_areas_found; i++) {
        // Transform the coords from (-1000, 1000)
        // to (0, previewWidth or previewHeight).
        af_roi_value.roi[i].left =
            (int32_t)(((double)areas[i].rect.left + 1000.0) *
                ((double)streamSize.w / 2000.0));
        af_roi_value.roi[i].top =
            (int32_t)(((double)areas[i].rect.top + 1000.0) *
                ((double)streamSize.h/ 2000.0));
        af_roi_value.roi[i].width =
            (int32_t)((double)areas[i].rect.width *
                (double)streamSize.w / 2000.0);
        af_roi_value.roi[i].height =
            (int32_t)((double)areas[i].rect.height *
                (double)streamSize.h/ 2000.0);
        af_roi_value.weight[i] = areas[i].weight;
    }

    free(areas);

    return NO_ERROR;
}

};
