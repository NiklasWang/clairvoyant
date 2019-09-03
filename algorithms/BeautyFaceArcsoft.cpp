#include <list>
#include <string>
#include <sstream>
#include <time.h>

#include "Common.h"
#include "BeautyFace.h"
#include "BeautyFaceArcsoft.h"
#include "AlgorithmHelper.h"
#include "PfLogger.h"

#include "FontWaterMark.h"
#include "Algorithm.h"

namespace pandora {

BeautyFaceArcsoft::BeautyFaceArcsoft(FrameType type):
    mInited(false),
    mModule(MODULE_ALGORITHM),
    mType(type),
    mDebug(false),
    mEngine(NULL)
{
    memset(&mFaceInput,   0, sizeof(mFaceInput));
    memset(&ImgDataInput, 0, sizeof(ImgDataInput));
}

BeautyFaceArcsoft::~BeautyFaceArcsoft()
{
    if(mInited) {
        deinit();
        mInited = false;
    }
}

int32_t runConfigTest();

int32_t BeautyFaceArcsoft::init()
{
    int32_t rc = NO_ERROR;
    if (mInited) {
        rc = ALREADY_EXISTS;
    }

    if (SUCCEED(rc)) {
        rc = runConfigTest();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Configuration error, check log, %d", rc);
            RESETRESULT(rc);
        }
    }

    if (SUCCEED(rc)) {
        if (ISNULL(mEngine)) {
            if (/*vendor::Create_BeautyShot_Algorithm(
                    vendor::BeautyShot_Algorithm::CLASSID,
                    &mEngine) != MOK || */ISNULL(mEngine)) {
                LOGE(mModule, "Failed to create arcsoft algorithm.");
                rc = NOT_INITED;
            }
        }
    }

    if (SUCCEED(rc)) {
        rc = mEngine->Init(mType == FRAME_TYPE_SNAPSHOT ?
            vendor::DataTypeImage : vendor::DataTypeVideo);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to init beauty engine, 0x%x", rc);
        }
    }

    if (SUCCEED(rc)) {
        LOGE(mModule, "Beautyshot Version:%s, %s mode.",
            mEngine->GetVersion(),
            mType == FRAME_TYPE_SNAPSHOT ? "Snapshot" : "Preview");
        mInited = true;
    }

    return rc;
}

int32_t BeautyFaceArcsoft::deinit()
{
    int32_t rc = NO_ERROR;

    mEngine->Uninit();
    mEngine->Release();
    mInited = false;

    return rc;
}

int32_t getAutoConfig(CameraTypeInf::CameraType camId, FrameType type,
    int32_t age, BeautyFace::Gender gender, int32_t luxIndex, bool backlight,
    BeautyFaceArcsoft::ParmMap<BeautyFaceArcsoft::BeautyFaceParmItem, int32_t> *result);

int32_t getManualConfig(CameraTypeInf::CameraType camId, FrameType type, int32_t level,
    BeautyFaceArcsoft::ParmMap<BeautyFaceArcsoft::BeautyFaceParmItem, int32_t> *result);

int32_t BeautyFaceArcsoft::doProcess(unsigned char *frame,
    int32_t w, int32_t h, int32_t stride, int32_t scanline)
{
    int32_t rc = NO_ERROR;
    int32_t strideW = getAlignedStride(w, stride);
    uint8_t *yDat = getNV21YData(frame,w, h, stride, scanline);
    uint8_t *uvDat = getNV21UVData(frame, w, h, stride, scanline);
    CameraTypeInf::CameraType camId = mParm.camType.type;
    ParmMap<BeautyFaceParmItem, int32_t> config[BEAUTY_FACE_MAX_INVALID];

    ImgDataInput.i32Height = h;
    ImgDataInput.i32Width = w;
    ImgDataInput.u32PixelArrayFormat = ASVL_PAF_NV21;
    ImgDataInput.ppu8Plane[0] = yDat;
    ImgDataInput.pi32Pitch[0] = strideW;
    ImgDataInput.ppu8Plane[1] = uvDat;
    ImgDataInput.pi32Pitch[1] = strideW;

    if (SUCCEED(rc)) {
        mEngine->SetOnlyFaceModel(false);
        rc = mEngine->SetFaceInfo(&ImgDataInput, NULL);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to set face info, 0x%x", rc);
        }
    }

    if (SUCCEED(rc)) {
        memset(config, 0, sizeof(config));
        if (mParm.manualStrength) {
            rc = getManualConfig(camId, mType, mParm.manualStrength, config);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to get manual configuration, %d", rc);
            }
        } else {
            rc = getAutoConfig(camId, mType, mParm.age, mParm.gender,
                mParm.luxIndex, mParm.isBacklight, config);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to get auto configuration, %d", rc);
            }
        }
    }

    if (SUCCEED(rc)) {
        if (mDebug) {
            GlobalTaskType t(frame, w, h, stride, scanline,
                getFrameLen2(w, h, stride, scanline),
                FRAME_FORMAT_YUV_420_NV21, FRAME_TYPE_PREVIEW);

            rc = drawWaterMark(FRAME_TYPE_PREVIEW, mParm, t, mParm.manualStrength);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to draw preview water mark, %d", rc);
            }

            t.type = FRAME_TYPE_SNAPSHOT;
            rc = drawWaterMark(FRAME_TYPE_SNAPSHOT, mParm, t, mParm.manualStrength);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to draw snapshot water mark, %d", rc);
            }
        }
        RESETRESULT(rc);
    }

    if (SUCCEED(rc)) {
        mEngine->SetSkinSoftenLevel(0,     config[BEAUTY_FACE_SKIN_SOFTEN].value);
        mEngine->SetSkinBrightLevel(0,     config[BEAUTY_FACE_SKIN_WHITEN].value);
        mEngine->SetShineRemoveLevel(0,    config[BEAUTY_FACE_ANTI_SHINE].value);
        mEngine->SetTeethWhiteLevel(0,     config[BEAUTY_FACE_TEETH_WHITEN].value);
        mEngine->SetEyeEnlargementLevel(0, config[BEAUTY_FACE_EYE_ENLARGEMENT].value);
        mEngine->SetEyeBrightLevel(0,      config[BEAUTY_FACE_EYE_BRIGHTEN].value);
        mEngine->SetNoseHighlightLevel(0,  config[BEAUTY_FACE_TZONE_HIGHTLIGHT].value);
        mEngine->SetFaceSlenderLevel(0,    config[BEAUTY_FACE_SLENDER_FACE].value);
        mEngine->SetDeblemishState(0,      config[BEAUTY_FACE_DE_BLEMISH].value);
        mEngine->SetDepouchState(0,        config[BEAUTY_FACE_DE_POUCH].value);
    }

    if (SUCCEED(rc)) {
        rc = mEngine->Process(&ImgDataInput, &ImgDataInput);
        if (SUCCEED(rc)) {
            if (mType == FRAME_TYPE_SNAPSHOT) {
                LOGI(mModule, "Succeed to process beauty face in snapshot mode.");
            } else {
                LOGD(mModule, "Succeed to process beauty face in preview mode.");
            }
        } else {
            LOGE(mModule, "Failed to process beauty face, 0x%x, %s mode.",
                rc, mType == FRAME_TYPE_SNAPSHOT ? "Snapshot" : "Preview");
        }
    }

    return rc;
}

int32_t BeautyFaceArcsoft::setParm(BeautyFaceParm &parm)
{
    mParm = parm;
    return NO_ERROR;
}

#define DEBUG_STR_MAX_LEN   255
#define WATERMARK_PREVIEW_FONT_SIZE  20
#define WATERMARK_SNAPSHOT_FONT_SIZE 60

int32_t getShiftConfigByTime(CameraTypeInf::CameraType camId, FrameType type,
    const BeautyFaceArcsoft::ParmMap<BeautyFaceArcsoft::BeautyFaceParmItem, int32_t> **config);

int32_t getShiftConfigByLight(CameraTypeInf::CameraType camId, int32_t luxIndex, FrameType type,
    const BeautyFaceArcsoft::ParmMap<BeautyFaceArcsoft::BeautyFaceParmItem, int32_t> **config);

int32_t getShiftConfigByBacklight(CameraTypeInf::CameraType camId, bool isBacklight, FrameType type,
    const BeautyFaceArcsoft::ParmMap<BeautyFaceArcsoft::BeautyFaceParmItem, int32_t> **config);

int32_t getAutoDefaultConfig(CameraTypeInf::CameraType camId, FrameType type,
    int32_t age, BeautyFace::Gender gender,
    const BeautyFaceArcsoft::ParmMap<BeautyFaceArcsoft::BeautyFaceParmItem, int32_t> **config);

int32_t BeautyFaceArcsoft::drawWaterMark(
    FrameType type, BeautyFaceParm &parm, GlobalTaskType &task, bool manual)
{
    return manual ?
        drawManualWaterMark(type, parm, task) :
        drawAutoWaterMark(type, parm, task);
}

int32_t BeautyFaceArcsoft::drawAutoWaterMark(
    FrameType type, BeautyFaceParm &parm, GlobalTaskType &task)
{
    int32_t rc = NO_ERROR;
    bool needMirror = false;
    CameraTypeInf::CameraType camId = mParm.camType.type;
    std::list<AlgTraits<FontWaterMark>::ParmType::TextType> *list = NULL;

    if (SUCCEED(rc)) {
        if (ISNULL(mWM[type])) {
            mWM[type] = Algorithm<FontWaterMark>::create(
                "BeautyFaceWatermark",
                "BeautyFaceWatermarkThread",
                false, 1, 2,
                type == FRAME_TYPE_SNAPSHOT ?
                    FONT_WATER_MARK_POSITION_BOTTOM_RIGHT :
                    FONT_WATER_MARK_POSITION_TOP_LEFT,
                ROTATION_ANGLE_0,
                "STOP");
            if (ISNULL(mWM[type])) {
                LOGE(mModule, "Failed to create font water mark algorithm");
                rc = INTERNAL_ERROR;
            }
        }
    }

    if (SUCCEED(rc)) {
        if (parm.camType.isFrontCamera() &&
            mType == FRAME_TYPE_PREVIEW) {
            needMirror = true;
        }
    }

    if (SUCCEED(rc)) {
        const BeautyFaceArcsoft::ParmMap<
            BeautyFaceArcsoft::BeautyFaceParmItem, int32_t> *defaultConfig = NULL;
        const BeautyFaceArcsoft::ParmMap<
            BeautyFaceArcsoft::BeautyFaceParmItem, int32_t> *timeConfig = NULL;
        const BeautyFaceArcsoft::ParmMap<
            BeautyFaceArcsoft::BeautyFaceParmItem, int32_t> *lightConfig = NULL;
        const BeautyFaceArcsoft::ParmMap<
            BeautyFaceArcsoft::BeautyFaceParmItem, int32_t> *backlightConfig = NULL;
        char header[DEBUG_STR_MAX_LEN];
        std::stringstream defaultStr, timeStr, lightStr, backlightStr, resultStr;

        const BeautyFaceArcsoft::ParmMap<
            BeautyFaceArcsoft::BeautyFaceParmItem, int32_t> zeroConfig[] = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,         0, },
        };

        if (SUCCEED(rc)) {
            rc = getAutoDefaultConfig(camId, type, parm.age, parm.gender, &defaultConfig);
            if (!SUCCEED(rc) || ISNULL(defaultConfig)) {
                LOGE(MODULE_ALGORITHM, "Failed to get default conf, %d", rc);
                rc = PARAM_INVALID;
            } else {
                defaultStr << "Default:";
                for (int32_t i = 0; i < BeautyFaceArcsoft::BEAUTY_FACE_MAX_INVALID; i++) {
                    defaultStr << "-" << defaultConfig[i].value;
                }
            }
        }

        if (SUCCEED(rc)) {
            rc = getShiftConfigByTime(camId, type, &timeConfig);
            if (!SUCCEED(rc) || ISNULL(timeConfig)) {
                LOGD(MODULE_ALGORITHM, "Failed to get time shift conf, %d", rc);
                timeConfig = zeroConfig;
                RESETRESULT(rc);
            }
            timeStr << "Time   :";
            for (int32_t i = 0; i < BeautyFaceArcsoft::BEAUTY_FACE_MAX_INVALID; i++) {
                timeStr << "-" << timeConfig[i].value;
            }
        }

        if (SUCCEED(rc)) {
            rc = getShiftConfigByLight(camId, parm.luxIndex, type, &lightConfig);
            if (!SUCCEED(rc) || ISNULL(lightConfig)) {
                lightConfig = zeroConfig;
                LOGD(MODULE_ALGORITHM, "Failed to get light shift conf, %d", rc);
                RESETRESULT(rc);
            }
            lightStr << "Light  :";
            for (int32_t i = 0; i < BeautyFaceArcsoft::BEAUTY_FACE_MAX_INVALID; i++) {
                lightStr << "-" << lightConfig[i].value;
            }
        }

        if (SUCCEED(rc)) {
            rc = getShiftConfigByBacklight(camId, parm.isBacklight, type, &backlightConfig);
            if (!SUCCEED(rc) || ISNULL(backlightConfig)) {
                LOGD(MODULE_ALGORITHM, "Failed to get backlight shift conf, %d", rc);
                backlightConfig = zeroConfig;
                RESETRESULT(rc);
            }
            backlightStr << "Backlig:";
            for (int32_t i = 0; i < BeautyFaceArcsoft::BEAUTY_FACE_MAX_INVALID; i++) {
                backlightStr << "-" << backlightConfig[i].value;
            }
        }

        if (SUCCEED(rc)) {
            ParmMap<BeautyFaceParmItem, int32_t> result[BEAUTY_FACE_MAX_INVALID];
            rc = getAutoConfig(camId, type, parm.age, parm.gender,
                parm.luxIndex, parm.isBacklight, result);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to get auto configuration, %d", rc);
            } else {
                resultStr  << "Result :";
                for (int32_t i = 0; i < BeautyFaceArcsoft::BEAUTY_FACE_MAX_INVALID; i++) {
                    resultStr  << "-" << result[i].value;
                }
            }
        }

        if (SUCCEED(rc)) {
            time_t now ;
            time(&now) ;
            struct tm tm_now ;
            localtime_r(&now, &tm_now);
            snprintf(header, DEBUG_STR_MAX_LEN,
                "%s, Age %d, %s, hr %d, Lux %d %s",
                type == FRAME_TYPE_SNAPSHOT ? "Snapshot" : "Preview",
                parm.age, BeautyFace::getGenderName(parm.gender),
                tm_now.tm_hour, parm.luxIndex,
                parm.isBacklight ? "backlight" : "");
        }

        if (SUCCEED(rc)) {
            int32_t size = mType == FRAME_TYPE_SNAPSHOT ?
                WATERMARK_SNAPSHOT_FONT_SIZE : WATERMARK_PREVIEW_FONT_SIZE;
            AlgTraits<FontWaterMark>::ParmType::TextType text;
            list = new std::list<AlgTraits<FontWaterMark>::ParmType::TextType>();
            text = { std::string("Powered by Pandora."), size };
            list->push_back(text);
            text = { header,             size };
            list->push_back(text);
            text = { defaultStr.str(),   size };
            list->push_back(text);
            text = { timeStr.str(),      size };
            list->push_back(text);
            text = { lightStr.str(),     size };
            list->push_back(text);
            text = { backlightStr.str(), size };
            list->push_back(text);
            text = { resultStr.str(),    size };
            list->push_back(text);
            text = { std::string("Don't worry. Will remove after debug."), size };
            list->push_back(text);
        }
    }

    if (SUCCEED(rc)) {
        PfLogger::getInstance()->start("beauty_water_mark_config", 10);
        if (NOTNULL(mWM[type])) {
            AlgTraits<FontWaterMark>::ParmType parm;
            parm.rotation   = mParm.rotation;
            parm.position   = FONT_WATER_MARK_POSITION_MAX_INVALID;
            parm.needMirror = needMirror;
            parm.texts      = list;
            rc = mWM[type]->config(parm);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to config %s, %d",
                    mWM[type]->whoamI(), rc);
            }
        }
        PfLogger::getInstance()->stop("beauty_water_mark_config");
    }

    if (SUCCEED(rc)) {
        PfLogger::getInstance()->start("beauty_water_mark_process", 10);
        if (NOTNULL(mWM[type])) {
            rc = mWM[type]->process(task);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to draw text, %d", rc);
                rc = INTERNAL_ERROR;
            }
        }
        PfLogger::getInstance()->stop("beauty_water_mark_process");
    }

    if (SUCCEED(rc) || !SUCCEED(rc)) {
        if (NOTNULL(list)) {
            SECURE_DELETE(list);
        }
    }

    return rc;
}

int32_t BeautyFaceArcsoft::drawManualWaterMark(
    FrameType type, BeautyFaceParm &parm, GlobalTaskType &task)
{
    int32_t rc = NO_ERROR;
    bool needMirror = false;
    CameraTypeInf::CameraType camId = mParm.camType.type;
    std::stringstream header, resultStr;
    std::list<AlgTraits<FontWaterMark>::ParmType::TextType> *list = NULL;

    if (SUCCEED(rc)) {
        if (ISNULL(mWM[type])) {
            mWM[type] = Algorithm<FontWaterMark>::create(
                "BeautyFaceWatermark",
                "BeautyFaceWatermarkThread",
                false, 1, 2,
                type == FRAME_TYPE_SNAPSHOT ?
                    FONT_WATER_MARK_POSITION_BOTTOM_RIGHT :
                    FONT_WATER_MARK_POSITION_TOP_LEFT,
                ROTATION_ANGLE_0,
                "STOP");
            if (ISNULL(mWM[type])) {
                LOGE(mModule, "Failed to create font water mark algorithm");
                rc = INTERNAL_ERROR;
            }
        }
    }

    if (SUCCEED(rc)) {
        if (parm.camType.isFrontCamera() &&
            mType == FRAME_TYPE_PREVIEW) {
            needMirror = true;
        }
    }

    if (SUCCEED(rc)) {
        ParmMap<BeautyFaceParmItem, int32_t> result[BEAUTY_FACE_MAX_INVALID];
        rc = getManualConfig(camId, type, parm.manualStrength, result);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to get auto configuration, %d", rc);
        } else {
            header << "Manual mode, strength " << parm.manualStrength;
            resultStr  << "Result :";
            for (int32_t i = 0; i < BeautyFaceArcsoft::BEAUTY_FACE_MAX_INVALID; i++) {
                resultStr  << "-" << result[i].value;
            }
        }
    }


    if (SUCCEED(rc)) {
        int32_t size = mType == FRAME_TYPE_SNAPSHOT ?
            WATERMARK_SNAPSHOT_FONT_SIZE : WATERMARK_PREVIEW_FONT_SIZE;
        AlgTraits<FontWaterMark>::ParmType::TextType text;
        list = new std::list<AlgTraits<FontWaterMark>::ParmType::TextType>();
        text = { std::string("Powered by Pandora."), size };
        list->push_back(text);
        text = { header.str(),    size };
        list->push_back(text);
        text = { resultStr.str(), size };
        list->push_back(text);
        text = { std::string("Don't worry. Will remove after debug."), size };
        list->push_back(text);
    }

    if (SUCCEED(rc)) {
        PfLogger::getInstance()->start("beauty_water_mark_config", 10);
        if (NOTNULL(mWM[type])) {
            AlgTraits<FontWaterMark>::ParmType parm;
            parm.rotation   = mParm.rotation;
            parm.position   = FONT_WATER_MARK_POSITION_MAX_INVALID;
            parm.needMirror = needMirror;
            parm.texts      = list;
            rc = mWM[type]->config(parm);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to config %s, %d",
                    mWM[type]->whoamI(), rc);
            }
        }
        PfLogger::getInstance()->stop("beauty_water_mark_config");
    }

    if (SUCCEED(rc)) {
        PfLogger::getInstance()->start("beauty_water_mark_process", 10);
        if (NOTNULL(mWM[type])) {
            rc = mWM[type]->process(task);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to draw text, %d", rc);
                rc = INTERNAL_ERROR;
            }
        }
        PfLogger::getInstance()->stop("beauty_water_mark_process");
    }

    if (SUCCEED(rc) || !SUCCEED(rc)) {
        if (NOTNULL(list)) {
            SECURE_DELETE(list);
        }
    }

    return rc;
}


};

