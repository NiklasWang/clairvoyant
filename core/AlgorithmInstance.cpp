#include "AlgorithmIntf.h"
#include "PandoraCore.h"
#include "AlgorithmProperties.h"
/*
#include "BeautyFace.h"
#include "AgeGenderDetection.h"
#include "HDRCheckerT.h"
#include "HDRT.h"
#include "HDRCheckerM.h"
#include "HDRM.h"
#include "HDRM4.h"
#include "SuperResolution.h"
#include "PhotoSolid5.h"
#include "SmartSelect.h"
#include "SmartShot.h"
#include "MovieSolid.h"
#include "PictureZoom.h"
#include "FairLight.h"
#include "MicroPlastic.h"
#include "PictureZoom2.h"
#include "StillBokeh.h"
#include "RTBokeh.h"
#include "StillBokehA.h"
#include "DualCamNightShot.h"
#include "SingleBokeh.h"
*/
#include "WaterMark.h"

#include "Algorithm.h"

namespace pandora {

sp<IAlgorithm> PandoraCore::getAlgorithm(AlgType type)
{
    int32_t rc = NO_ERROR;
    sp<IAlgorithm> result = NULL;

    if (SUCCEED(rc)) {
        RWLock::AutoRLock l(mAlgLock);
        for (auto &info : mAlgorithms) {
            if (info.alg->getType() == type) {
                result = info.alg;
                break;
            }
        }
    }

    if (SUCCEED(rc) && ISNULL(result)) {
        mAlgProperties = ISNULL(mAlgProperties) ?
            new AlgProperties() : mAlgProperties;
        if (ISNULL(mAlgProperties)) {
            LOGE(mModule, "Failed to create algorithm properties");
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc) && ISNULL(result)) {
        std::string name = mAlgProperties->getName(type);
        std::string threadName = mAlgProperties->getThreadName(type);
        bool frameDrop = mAlgProperties->isDropFrame(type);
        uint32_t maxQueueSize = mAlgProperties->getMaxQueueSize(type);
        uint32_t argsNum = mAlgProperties->getArgsNum(type);

        switch (type) {
#if 0
#ifdef ENABLE_BEAUTY_FACE
            case ALG_BEAUTY_FACE: {
                result = Algorithm<BeautyFace>::create(
                    name, threadName, frameDrop,
                    maxQueueSize, argsNum, ALG_BEAUTY_FACE);
            } break;
            case ALG_BEAUTY_FACE_CAP: {
                result = Algorithm<BeautyFace>::create(
                    name, threadName, frameDrop,
                    maxQueueSize, argsNum, ALG_BEAUTY_FACE_CAP);
            } break;
#endif
#ifdef ENABLE_GENDER_DETECTION
            case ALG_AGE_GENDER_DETECTION: {
                result = Algorithm<AgeGenderDetection>::create(
                    name, threadName, frameDrop, maxQueueSize, argsNum);
            } break;
#endif
#ifdef ENABLE_IMAGE_STABILICATION
            case ALG_IMAGE_STABILIZATION: {
                result = Algorithm<PhotoSolid>::create(
                    name, threadName, frameDrop, maxQueueSize,
                    argsNum, mCamStatus.pictureSize.w, mCamStatus.pictureSize.h);
            } break;
#endif
#ifdef ENABLE_PICTURE_ZOOM
            case ALG_PICTURE_ZOOM: {
                int width = mCamStatus.platformPriv.w;
                int height = mCamStatus.platformPriv.h;
                result = Algorithm<PictureZoom>::create(
                    name, threadName, frameDrop, maxQueueSize, argsNum, width, height);
            } break;
#endif
#ifdef ENABLE_FAIR_LIGHT
            case ALG_FAIR_LIGHT: {
                result = Algorithm<FairLight>::create(
                    name, threadName, frameDrop, maxQueueSize, argsNum);
            } break;
#endif
#ifdef ENABLE_MICRO_PLASTIC
            case ALG_MICRO_PLASTIC: {
                LOGE(mModule, "microplastic create");
                result = Algorithm<MicroPlastic>::create(
                    name, threadName, frameDrop, maxQueueSize, argsNum);
            } break;
#endif
#ifdef ENABLE_PICTURE_ZOOM2
            case ALG_PICTURE_ZOOM2: {
                result = Algorithm<PictureZoom2>::create(
                    name, threadName, frameDrop, maxQueueSize, argsNum);
            } break;
#endif
#ifdef ENABLE_HDR
            case ALG_HDR_CHECKER: {
                result = Algorithm<HDRChecker>::create(
                    name, threadName, frameDrop, maxQueueSize, argsNum);
            } break;
            case ALG_HDR: {
                result = Algorithm<HDR>::create(
                    name, threadName, frameDrop, maxQueueSize, argsNum);
            } break;
#endif
#ifdef ENABLE_HDRM
            case ALG_HDR_CHECKER: {
                result = Algorithm<HDRChecker>::create(
                    name, threadName, frameDrop, maxQueueSize, argsNum);
            } break;
            case ALG_HDR: {
                result = Algorithm<HDR>::create(
                    name, threadName, frameDrop, maxQueueSize, argsNum);
            } break;
#endif
#ifdef ENABLE_HDRM4
            case ALG_HDR_CHECKER: {
                result = Algorithm<HDRChecker>::create(
                    name, threadName, frameDrop, maxQueueSize, argsNum);
            } break;
            case ALG_HDR: {
                result = Algorithm<HDRM4>::create(
                    name, threadName, frameDrop, maxQueueSize, argsNum);
            } break;
#endif
#ifdef ENABLE_SUPER_RESOLUTION
            case ALG_SUPER_RESOLUTION: {
                result = Algorithm<SuperResolution>::create(
                    name, threadName, frameDrop, maxQueueSize, argsNum);
            } break;
#endif
#ifdef ENABLE_SMART_SELECT
            case ALG_SMART_SELECT: {
                result = Algorithm<SmartSelect>::create(
                    name, threadName, frameDrop, maxQueueSize, argsNum);
            } break;
#endif
#ifdef ENABLE_SMART_SHOT
            case ALG_SMART_SHOT: {
                result = Algorithm<SmartShot>::create(
                    name, threadName, frameDrop, maxQueueSize, argsNum);
            } break;
#endif
#ifdef ENABLE_VIDEO_STABILIZATION
            case ALG_VIDEO_STAB_PREVIEW: {
                result = Algorithm<MovieSolid>::create(
                    name, threadName, frameDrop, maxQueueSize,
                    argsNum, ALG_VIDEO_STAB_PREVIEW);
            } break;
            case ALG_VIDEO_STAB_RECORDING: {
                int cameraId = checkFrontCamera();
                int videoFlip = mCamStatus.flipMode.v || mCamStatus.flipMode.h;
                result = Algorithm<MovieSolid>::create(
                    name, threadName, frameDrop, maxQueueSize,
                    argsNum, ALG_VIDEO_STAB_RECORDING, cameraId, videoFlip);
            } break;
#endif
#ifdef ENABLE_STILL_BOKEH
            case ALG_STILL_BOKEH: {
                result = Algorithm<StillBokeh>::create(
                    name, threadName, frameDrop, maxQueueSize, argsNum);
            } break;
#endif
#ifdef ENABLE_RT_BOKEH
            case ALG_RT_BOKEH: {
                result = Algorithm<RTBokeh>::create(
                    name, threadName, frameDrop, maxQueueSize, argsNum);
            } break;
#endif
#ifdef ENABLE_STILL_BOKEHA
            case ALG_STILL_BOKEHA: {
                result = Algorithm<StillBokehA>::create(
                    name, threadName, frameDrop, maxQueueSize, argsNum);
            } break;
#endif
#ifdef ENABLE_DUAL_NIGHT
            case ALG_DUAL_CAM_NIGHT_SHOT: {
                result = Algorithm<DualCamNightShot>::create(
                    name, threadName, frameDrop, maxQueueSize, argsNum);
            } break;
#endif
#ifdef ENABLE_NIGHT_SHOT
            case ALG_NIGHT_SHOT: {
                result = Algorithm<NightShot>::create(
                    name, threadName, frameDrop, maxQueueSize, argsNum);
            } break;
#endif
#ifdef ENABLE_ANTI_SHAKE
            case ALG_ANTI_SHAKING: {
                result = Algorithm<AntiShaking>::create(
                    name, threadName, frameDrop, maxQueueSize, argsNum);
            } break;
#endif
#ifdef ENABLE_SINGLE_BOKEH
            case ALG_SINGLE_BOKEH: {
                result = Algorithm<SingleBokeh>::create(
                    name, threadName, frameDrop, maxQueueSize,
                    argsNum, ALG_SINGLE_BOKEH);
            } break;
            case ALG_SINGLE_BOKEH_CAP: {
                result = Algorithm<SingleBokeh>::create(
                    name, threadName, frameDrop, maxQueueSize,
                    argsNum, ALG_SINGLE_BOKEH_CAP);
            } break;
#endif
#endif
            case ALG_MAX_INVALID:
            default: {
                LOGE(mModule, "Invalid algorithm type %d", type);
            } break;
        }

        if (NOTNULL(result)) {
            result->addListener(pthread_self(), this);
            AlgorithmInfo info = {
                .alg = result,
                .type = type,
                .name = result->getName(),
                .caps = result->queryCap(),
                .frameCnt = 0,
            };

            mAlgMask |= getAlgMask(type);
            RWLock::AutoWLock l(mAlgLock);
            mAlgorithms.push_back(info);
        }
    }

    if (ISNULL(result)) {
        LOGE(mModule, "Failed to create algorithm type %d", type);
    }

    return result;
}

};

