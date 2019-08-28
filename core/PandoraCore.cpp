#include "PandoraIntfInternal.h"
#include "PandoraCore.h"
#include "FrameReorder.h"
#include "FrameCounter.h"
#include "PalTester.h"
#include "ThreadPoolEx.h"

#include "JpegSWEncoder.h"
#include "JpegExif.h"
#include "WaterMark.h"

#include "Algorithm.h"

namespace pandora {

PandoraCore::PandoraCore(
    PlatformOpsIntf *platform) :
    mConstructed(false),
    mModule(MODULE_PANDORA_CORE),
    mPlatform(platform),
    mPal(NULL),
    mAlgMask(ALG_MASK_NONE_ALORITHM),
    mFrameReorder(NULL),
    mFrameCounter(NULL),
    mBufferedPair(5),
    mExtEvtId(-1),
    mPalTester(NULL),
    mThreads(NULL),
    mExifReader(NULL),
    mAsyncSnapshot(false),
    mTestAlgIntf(false),
    mTestPal(false)
{
}

int32_t PandoraCore::construct()
{
    int32_t rc = NO_ERROR;

    if (mConstructed) {
        rc = ALREADY_EXISTS;
    }

    if (SUCCEED(rc)) {
        mPal = new Pal(mPlatform);
        if (ISNULL(mPal)) {
            LOGE(mModule, "Failed to create pal");
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        rc = mPal->construct();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to construct pal");
        }
    }

    if (SUCCEED(rc)) {
        rc = mStatusMgr.setPal(mPal);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to set pal to status manager");
        }
    }

    if (SUCCEED(rc)) {
        if (ISNULL(mThreads)) {
            mThreads = ThreadPoolEx::getInstance();
            if (ISNULL(mThreads)) {
                LOGE(mModule, "Failed to get thread pool");
                rc = UNKNOWN_ERROR;
            }
        }
    }

    if (SUCCEED(rc)) {
        for (int32_t i = 0; i < EEVT_MAX_INVALID; i++) {
            mEvents[i].evt = mEvents[i].type = mEvents[i].arg = 0;
        }
    }

    if (SUCCEED(rc)) {
        if (ISNULL(mPalTester)) {
            mPalTester = new PalTester(mPal);
            if (ISNULL(mPalTester)) {
                LOGE(mModule, "Failed to new pal tester.");
            }
        }
    }

    if (SUCCEED(rc)) {
        // Wait until test finished
        // Camera status will keep changing when camera started preview
        rc = mThreads->runWait(
            [this]() -> int32_t {
                int32_t _rc = NO_ERROR;
                _rc = mTestPal ? mPalTester->run() : NO_ERROR;
                if (!SUCCEED(_rc)) {
                    LOGE(mModule, "Test Failed! During running "
                        "Pal test cases, check logs, %d", _rc);
                }
                return _rc;
            }
        );
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to run Pal test cases, "
                "ignore, %d", rc);
            RESETRESULT(rc);
        }
    }

    if (SUCCEED(rc)) {
        if (ISNULL(mFrameCounter)) {
            mFrameCounter = new FrameCounter();
            if (ISNULL(mFrameCounter)) {
                LOGE(mModule, "Failed to new frame counter.");
            }
        }
    }

    if (SUCCEED(rc)) {
        if (mAsyncSnapshot) {
            if (ISNULL(mExifReader)) {
                mExifReader= Algorithm<JpegExif>::create(
                    "JpegExif", "JpegExif", false, 1, 0);
                if (ISNULL(mExifReader)) {
                    LOGE(mModule, "Failed to create exifReader algorithm");
                    rc = INTERNAL_ERROR;
                }
            }
        }
    }


    if (SUCCEED(rc)) {
        RWLock::AutoRLock l(mMultiExposureLock);
        mMultiExposure.clear();
    }

    if (SUCCEED(rc)) {
        mConstructed = true;
        LOGD(mModule, "Pandora core constructed");
    }

    return RETURNIGNORE(rc, ALREADY_EXISTS);
}

int32_t PandoraCore::destruct()
{
    int32_t rc = NO_ERROR;

    if (!mConstructed) {
        rc = NOT_INITED;
    } else {
        mConstructed = false;
    }

    if (SUCCEED(rc)) {
        if (NOTNULL(mPalTester)) {
            SECURE_DELETE(mPalTester);
        }
    }

    if (SUCCEED(rc)) {
        if (NOTNULL(mFrameCounter)) {
            SECURE_DELETE(mFrameCounter);
        }
    }

    if (SUCCEED(rc)) {
        if (NOTNULL(mExifReader)) {
            SECURE_DELETE(mExifReader);
        }
    }

    if (SUCCEED(rc)) {
        rc = mPal->destruct();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to destruct pal");
        }
    }

    if (SUCCEED(rc)) {
        if (NOTNULL(mPal)) {
            delete mPal;
            mPal = NULL;
        }
    }

    if (NOTNULL(mThreads)) {
        mThreads->removeInstance();
    }


    if (!SUCCEED(rc)) {
        mConstructed = true;
        LOGE(mModule, "Failed to destruct pandora core");
    } else {
        LOGD(mModule, "Pandora core destructed");
    }

    return RETURNIGNORE(rc, NOT_INITED);
}

PandoraCore::~PandoraCore()
{
    for (int32_t i = 0; i < ALG_MAX_INVALID; i++) {
        mAlgStatus[i].enabled = false;
    }

    RWLock::AutoWLock l(mAlgLock);
    auto iter = mAlgorithms.begin();
    while (iter != mAlgorithms.end()) {
        mAlgMask &= ~(getAlgMask(iter->alg->getType()));
        mAlgorithms.erase(iter);
        iter = mAlgorithms.begin();
    }

    if (NOTNULL(mFrameReorder)) {
        mFrameReorder->removeInstance();
    }
}

int32_t PandoraCore::startPreview()
{
    int32_t rc = NO_ERROR;

    if (mAlgStatus[ALG_BEAUTY_FACE].enable) {
        sp<IAlgorithm> alg = getAlgorithm(ALG_BEAUTY_FACE);
        if (NOTNULL(alg)) {
            mAlgStatus[ALG_BEAUTY_FACE].enabled = true;
        } else {
            LOGE(mModule, "Failed to create beauty face algorithm");
        }
    } else {
        mAlgStatus[ALG_BEAUTY_FACE].enabled = false;
    }

    if (mAlgStatus[ALG_AGE_GENDER_DETECTION].enable) {
        sp<IAlgorithm> alg = getAlgorithm(ALG_AGE_GENDER_DETECTION);
        if (NOTNULL(alg)) {
            mAlgStatus[ALG_AGE_GENDER_DETECTION].enabled = true;
        } else {
            LOGE(mModule, "Failed to create age gender detection");
        }
    } else {
        mAlgStatus[ALG_AGE_GENDER_DETECTION].enabled = false;
    }

    if (mAlgStatus[ALG_MICRO_PLASTIC].enable) {
        sp<IAlgorithm> alg = getAlgorithm(ALG_MICRO_PLASTIC);
        if (NOTNULL(alg)) {
            mAlgStatus[ALG_MICRO_PLASTIC].enabled = true;
        } else {
            LOGE(mModule, "Failed to create micro plastic");
        }
    } else {
        mAlgStatus[ALG_MICRO_PLASTIC].enabled = false;
    }

    if (mAlgStatus[ALG_HDR_CHECKER].enable) {
        sp<IAlgorithm> alg = getAlgorithm(ALG_HDR_CHECKER);
        if (NOTNULL(alg)) {
            mAlgStatus[ALG_HDR_CHECKER].enabled = true;
        } else {
            LOGE(mModule, "Failed to create hdr checker algorithm");
        }
    } else {
        mAlgStatus[ALG_HDR_CHECKER].enabled = false;
    }

    if (mAlgStatus[ALG_RT_BOKEH].enable) {
        sp<IAlgorithm> alg = getAlgorithm(ALG_RT_BOKEH);
        if (NOTNULL(alg)) {
            mAlgStatus[ALG_RT_BOKEH].enabled = true;
        } else {
            LOGE(mModule, "Failed to create RT Bokeh algorithm");
        }
    } else {
        mAlgStatus[ALG_RT_BOKEH].enabled = false;
    }

    if (mAlgStatus[ALG_VIDEO_STAB_PREVIEW].enable) {
        sp<IAlgorithm> alg = getAlgorithm(ALG_VIDEO_STAB_PREVIEW);
        if (NOTNULL(alg)) {
            mAlgStatus[ALG_VIDEO_STAB_PREVIEW].enabled = true;
        } else {
            LOGE(mModule, "Failed to create video stab "
                "preview algorithm");
        }
    } else {
        mAlgStatus[ALG_VIDEO_STAB_PREVIEW].enabled = false;
    }

    if (mAlgStatus[ALG_SINGLE_BOKEH].enable) {
        sp<IAlgorithm> alg = getAlgorithm(ALG_SINGLE_BOKEH);
        if (NOTNULL(alg)) {
            mAlgStatus[ALG_SINGLE_BOKEH].enabled = true;
        } else {
            LOGE(mModule, "Failed to create single bokeh algorithm");
        }
    } else {
        mAlgStatus[ALG_SINGLE_BOKEH].enabled = false;
    }

    return rc;
}

int32_t PandoraCore::stopPreview()
{
    int32_t rc = NO_ERROR;

    if (mAlgMask & getAlgMask(ALG_HDR_CHECKER)) {
        mAlgStatus[ALG_HDR_CHECKER].enabled = false;
        removeAlgorithm(ALG_HDR_CHECKER);
    }

    if (mAlgMask & getAlgMask(ALG_VIDEO_STAB_PREVIEW)) {
        mAlgStatus[ALG_VIDEO_STAB_PREVIEW].enabled = false;
        removeAlgorithm(ALG_VIDEO_STAB_PREVIEW);
    }

    return rc;
}

int32_t PandoraCore::takePicture()
{
    int32_t rc = NO_ERROR;
    uint32_t frameNeeded = 1;

    mCamStatus.takingPic = true;
    mCamStatus.snapshotNeeded = 1;
#if 0
    if (SUCCEED(rc)) {
        if (mAlgStatus[ALG_PICTURE_ZOOM].enable) {
            sp<IAlgorithm> alg = getAlgorithm(ALG_PICTURE_ZOOM);
            if (NOTNULL(alg)) {
                mAlgStatus[ALG_PICTURE_ZOOM].enabled = true;
                frameNeeded = alg->queryCap().frameNeeded;
                rc = JUMP_DONE;
            } else {
                LOGE(mModule, "Failed to create picture zoom");
                rc = UNKNOWN_ERROR;
            }
        } else {
            mAlgStatus[ALG_PICTURE_ZOOM].enabled = false;
        }
    }

    if (SUCCEED(rc)) {
        if (mAlgStatus[ALG_PICTURE_ZOOM2].enable) {
            sp<IAlgorithm> alg = getAlgorithm(ALG_PICTURE_ZOOM2);
            AlgTraits<PictureZoom2>::StatusType status;
            rc = alg->queryStatus(status);
            if (SUCCEED(rc)) {
                frameNeeded = status.rtFrameNeeded;
                mAlgStatus[ALG_PICTURE_ZOOM2].enabled = true;
                rc = JUMP_DONE;
            } else {
                LOGE(mModule, "Failed to create picture zoom2");
                rc = UNKNOWN_ERROR;
            }
        } else {
            mAlgStatus[ALG_PICTURE_ZOOM2].enabled = false;
        }
    }

    if (SUCCEED(rc)) {
        if (mAlgStatus[ALG_FAIR_LIGHT].enable) {
            sp<IAlgorithm> alg = getAlgorithm(ALG_FAIR_LIGHT);
            if (NOTNULL(alg)) {
                mAlgStatus[ALG_FAIR_LIGHT].enabled = true;
                rc = JUMP_DONE;
            } else {
                LOGE(mModule, "Failed to create fair light");
                rc = UNKNOWN_ERROR;
            }
        } else {
            mAlgStatus[ALG_FAIR_LIGHT].enabled = false;
        }
    }

    if (SUCCEED(rc)) {
        if (mAlgStatus[ALG_STILL_BOKEH].enable) {
            sp<IAlgorithm> alg = getAlgorithm(ALG_STILL_BOKEH);
            if (NOTNULL(alg)) {
                mAlgStatus[ALG_STILL_BOKEH].enabled = true;
                // Dual camera alg, divide 2
                frameNeeded = alg->queryCap().frameNeeded / 2;
                rc = JUMP_DONE;
            } else {
                LOGE(mModule, "Failed to create dual camera night shot");
                rc = UNKNOWN_ERROR;
            }
        } else {
            mAlgStatus[ALG_STILL_BOKEH].enabled = false;
        }
    }

    if (SUCCEED(rc)) {
        if (mAlgStatus[ALG_STILL_BOKEHA].enable) {
            sp<IAlgorithm> alg = getAlgorithm(ALG_STILL_BOKEHA);
            if (NOTNULL(alg)) {
                mAlgStatus[ALG_STILL_BOKEHA].enabled = true;
                frameNeeded = alg->queryCap().frameNeeded / 2;
                rc = JUMP_DONE;
            } else {
                LOGE(mModule, "Failed to create dual camera night shot");
                rc = UNKNOWN_ERROR;
            }
        } else {
            mAlgStatus[ALG_STILL_BOKEHA].enabled = false;
        }
    }

    if (SUCCEED(rc)) {
        if (mAlgStatus[ALG_DUAL_CAM_NIGHT_SHOT].enable) {
            sp<IAlgorithm> alg = getAlgorithm(ALG_DUAL_CAM_NIGHT_SHOT);
            if (NOTNULL(alg)) {
                mAlgStatus[ALG_DUAL_CAM_NIGHT_SHOT].enabled = true;
                frameNeeded = alg->queryCap().frameNeeded;
                rc = JUMP_DONE;
            } else {
                LOGE(mModule, "Failed to create dual camera night shot");
                rc = UNKNOWN_ERROR;
            }
        } else {
            mAlgStatus[ALG_DUAL_CAM_NIGHT_SHOT].enabled = false;
        }
    }

    if (SUCCEED(rc)) {
        if (mAlgStatus[ALG_HDR].enable) {
            sp<IAlgorithm> alg = getAlgorithm(ALG_HDR);
            if (NOTNULL(alg)) {
                mAlgStatus[ALG_HDR].enabled = true;
                rc = JUMP_DONE;
            } else {
                LOGE(mModule, "Failed to create HDR algorithm");
                rc = UNKNOWN_ERROR;
            }
        } else {
            mAlgStatus[ALG_HDR].enabled = false;
        }
    }

    if (SUCCEED(rc)) {
        if (mAlgStatus[ALG_IMAGE_STABILIZATION].enable) {
            sp<IAlgorithm> alg = getAlgorithm(ALG_IMAGE_STABILIZATION);
            if (NOTNULL(alg)) {
                mAlgStatus[ALG_IMAGE_STABILIZATION].enabled = true;
                frameNeeded = alg->queryCap().frameNeeded;
                rc = JUMP_DONE;
            } else {
                LOGE(mModule, "Failed to create image stabilization");
                rc = UNKNOWN_ERROR;
            }
        } else {
            mAlgStatus[ALG_IMAGE_STABILIZATION].enabled = false;
        }
    }

    if (SUCCEED(rc)) {
        if (mAlgStatus[ALG_SUPER_RESOLUTION].enable) {
            sp<IAlgorithm> alg = getAlgorithm(ALG_SUPER_RESOLUTION);
            if (NOTNULL(alg)) {
                AlgTraits<SuperResolution>::StatusType status;
                rc = alg->queryStatus(status);
                if (SUCCEED(rc)) {
                    frameNeeded = status.rtFrameNeeded;
                    mAlgStatus[ALG_SUPER_RESOLUTION].enabled = true;
                    rc = JUMP_DONE;
                } else {
                    LOGE(mModule, "Failed to query algorithm status");
                }
            } else {
                LOGE(mModule, "Failed to create image stabilization");
                rc = UNKNOWN_ERROR;
            }
        } else {
            mAlgStatus[ALG_SUPER_RESOLUTION].enabled = false;
        }
    }

    if (SUCCEED(rc)) {
        if (mAlgStatus[ALG_SMART_SHOT].enable) {
           sp<IAlgorithm> alg = getAlgorithm(ALG_SMART_SHOT);
           if (NOTNULL(alg)) {
               mAlgStatus[ALG_SMART_SHOT].enabled = true;
               frameNeeded = alg->queryCap().frameNeeded;
               rc = JUMP_DONE;
           } else {
               LOGE(mModule, "Failed to create smart shot");
               rc = UNKNOWN_ERROR;
           }
        } else {
            mAlgStatus[ALG_SMART_SHOT].enabled = false;
        }
    }

    if (SUCCEED(rc)) {
        if (mAlgStatus[ALG_SMART_SELECT].enable) {
            sp<IAlgorithm> alg = getAlgorithm(ALG_SMART_SELECT);
            if (NOTNULL(alg)) {
                mAlgStatus[ALG_SMART_SELECT].enabled = true;
                frameNeeded = alg->queryCap().frameNeeded;
                rc = JUMP_DONE;
            } else {
                LOGE(mModule, "Failed to create smart select");
                rc = UNKNOWN_ERROR;
            }
        } else {
            mAlgStatus[ALG_SMART_SELECT].enabled = false;
        }
    }

    if (SUCCEED(rc)) {
        if (mAlgStatus[ALG_ANTI_SHAKING].enable) {
            sp<IAlgorithm> alg = getAlgorithm(ALG_ANTI_SHAKING);
            if (NOTNULL(alg)) {
                mAlgStatus[ALG_ANTI_SHAKING].enabled = true;
                frameNeeded = alg->queryCap().frameNeeded;
                rc = JUMP_DONE;
            } else {
                LOGE(mModule, "Failed to create anti shaking");
                rc = UNKNOWN_ERROR;
            }
        } else {
            mAlgStatus[ALG_ANTI_SHAKING].enabled = false;
        }
    }

    if (SUCCEED(rc)) {
        if (mAlgStatus[ALG_NIGHT_SHOT].enable) {
            sp<IAlgorithm> alg = getAlgorithm(ALG_NIGHT_SHOT);
            if (NOTNULL(alg)) {
                mAlgStatus[ALG_NIGHT_SHOT].enable = true;
                frameNeeded = alg->queryCap().frameNeeded;
                rc = JUMP_DONE;
            } else {
                LOGE(mModule, "Failed to create night shot");
                rc = UNKNOWN_ERROR;
            }
        } else {
            mAlgStatus[ALG_NIGHT_SHOT].enabled = false;
        }
    }

    if (SUCCEED(rc)) {
        if (mAlgStatus[ALG_SINGLE_BOKEH_CAP].enable) {
            sp<IAlgorithm> alg = getAlgorithm(ALG_SINGLE_BOKEH_CAP);
            if (NOTNULL(alg)) {
                mAlgStatus[ALG_SINGLE_BOKEH_CAP].enabled= true;
                frameNeeded = alg->queryCap().frameNeeded;
                rc = JUMP_DONE;
            } else {
                LOGE(mModule, "Failed to create siglebokeh");
                rc = UNKNOWN_ERROR;
            }
        } else {
            mAlgStatus[ALG_SINGLE_BOKEH_CAP].enabled = false;
        }
    }
#endif
    if (rc == JUMP_DONE) {
        RESETRESULT(rc);
    }

    if (SUCCEED(rc)) {
        if (frameNeeded != 1) {
            MultiShotParm parm = {
                .num = frameNeeded,
            };

            rc = mPal->setParm(parm);
            if (SUCCEED(rc)) {
                mCamStatus.multishot = true;
                mCamStatus.snapshotNeeded = frameNeeded;
            } else {
                LOGE(mModule, "Failed to set multishot %d, %d", frameNeeded, rc);
            }
        }
    }

    if (SUCCEED(rc)) {
        if (mAlgStatus[ALG_HDR].enabled) {
            RWLock::AutoRLock l(mMultiExposureLock);
            rc = mPal->setParm(mMultiExposure);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to enable multi exposure, num %d, %d",
                    mMultiExposure.size(), rc);
            } else {
                mCamStatus.bracket = true;
                mCamStatus.snapshotNeeded = mMultiExposure.size();
            }
        }
    }

    if (SUCCEED(rc)) {
        mFrameCounter->setCameraMode(
            mCamStatus.dualCamMode.enabledNone() ?
                SingleCameraMode : DualCameraMode);
        mFrameCounter->reset();
        mFrameCounter->setTotalNum(mCamStatus.snapshotNeeded);
    }

    if (SUCCEED(rc)) {
        if (mCamStatus.multishot || mCamStatus.bracket) {
            sendEvtCallback(EEVT_ALG_PROCESSING);
        }
    }

    return rc;
}

int32_t PandoraCore::finishPicture()
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (mCamStatus.multishot || mCamStatus.bracket) {
            sendEvtCallback(EEVT_ALG_FINISHED);
        }
    }

    if (SUCCEED(rc)) {
        if (mCamStatus.bracket) {
            MultiExporeParm multiExposure;
            multiExposure.clear();
            rc = mPal->setParm(multiExposure);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to disable bracket, %d", rc);
            } else {
                mCamStatus.bracket = false;
            }
        }
    }

    if (SUCCEED(rc)) {
        if (mCamStatus.multishot) {
            MultiShotParm parm = {
                .num = 1,
            };
            rc = mPal->setParm(parm);
            mCamStatus.multishot = false;
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to disable bracket, %d", rc);
            }
        }
    }

    mAlgStatus[ALG_HDR].enabled          = false;
    mAlgStatus[ALG_SMART_SELECT].enabled = false;
    mAlgStatus[ALG_ANTI_SHAKING].enabled = false;
    mAlgStatus[ALG_NIGHT_SHOT].enabled   = false;
    mAlgStatus[ALG_SMART_SHOT].enabled   = false;
    mAlgStatus[ALG_IMAGE_STABILIZATION].enabled = false;
    mAlgStatus[ALG_SUPER_RESOLUTION].enabled    = false;
    mAlgStatus[ALG_PICTURE_ZOOM].enabled        = false;
    mAlgStatus[ALG_FAIR_LIGHT].enabled          = false;
    mAlgStatus[ALG_MICRO_PLASTIC].enabled       = false;
    mAlgStatus[ALG_PICTURE_ZOOM2].enabled       = false;
    mAlgStatus[ALG_NIGHT_SHOT].enabled          = false;

    mCamStatus.takingPic   = false;
    mCamStatus.snapshotNeeded = 1;
    mCamStatus.snapshotCnt = 0;

    return rc;
}

int32_t PandoraCore::startRecording()
{
    mCamStatus.recording = true;

    if (mAlgStatus[ALG_VIDEO_STAB_RECORDING].enable) {
        sp<IAlgorithm> alg = getAlgorithm(ALG_VIDEO_STAB_RECORDING);
        if (NOTNULL(alg)) {
            mAlgStatus[ALG_VIDEO_STAB_RECORDING].enabled = true;
        } else {
            LOGE(mModule, "Failed to create video stab video algorithm");
        }
    } else {
        mAlgStatus[ALG_VIDEO_STAB_RECORDING].enabled = false;
    }

    return NO_ERROR;
}

int32_t PandoraCore::stopRecording()
{
    mCamStatus.recording = false;

    if (mAlgMask & getAlgMask(ALG_VIDEO_STAB_RECORDING)) {
        mAlgStatus[ALG_VIDEO_STAB_RECORDING].enabled = false;
        removeAlgorithm(ALG_VIDEO_STAB_RECORDING);
    }

    return NO_ERROR;
}

bool PandoraCore::checkRecording()
{
    return mCamStatus.recording;
}

int32_t PandoraCore::onPreviewSizeChanged(
    PreviewDim & /*old*/, PreviewDim & /*latest*/)
{
    // Nothing to do
    return NO_ERROR;
}

int32_t PandoraCore::onPictureSizeChanged(
    PictureDim & /*old*/, PictureDim & /*latest*/)
{
    // Nothing to do
    return NO_ERROR;
}

int32_t PandoraCore::onVideoSizeChanged(
    VideoDim & /*old*/, VideoDim & /*latest*/)
{
    // Nothing to do
    return NO_ERROR;
}

int32_t PandoraCore::updateMetadata(
    ReqArgs<TT_METADATA_AVAILABLE> &task)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (NOTNULL(mPal)) {
            rc = mPal->onMetadataAvailable(task.ptr);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to notify metadata");
            }
        } else {
            rc = NOT_INITED;
        }
    }

    if (SUCCEED(rc)) {
        AEInfo ae;
        rc = mPal->getParm(ae);
        if (SUCCEED(rc)) {
            mStatusMgr.push_back(ae);
        }

        AFInfo af;
        rc = mPal->getParm(af);
        if (SUCCEED(rc)) {
            mStatusMgr.push_back(af);
        }

        RESETRESULT(rc);
    }

    if (SUCCEED(rc)) {
        bool isMacro = false;
        if (mStatusMgr.checkFocusChanged(isMacro)) {
            sendEvtCallback(EEVT_FOCUS_CHANGED, isMacro);
        }
    }

    if (SUCCEED(rc)) {
        CamStatusMgr::LumDir direction =
            CamStatusMgr::LUMINANCE_TOWARDS_UNKNOWN;
        if (mStatusMgr.checkExposureChanged(direction)) {
            sendEvtCallback(EEVT_LUMINANCE_CHANGED, direction);
        }
    }

    if (SUCCEED(rc)) {
        if (checkFlashChanged()) {
            if (mStatusMgr.checkAutoFlashOn()) {
                sendEvtCallback(EEVT_FLASH_STATUS, FLASH_ON_EVT);
            } else {
                sendEvtCallback(EEVT_FLASH_STATUS, FLASH_OFF_EVT);
            }
        }
    }

    if (SUCCEED(rc)) {
        bool isLowLight = false;
        if (mStatusMgr.checkLowLightChanged(isLowLight)) {
            sendEvtCallback(EEVT_LIGHT_STATUS,
                isLowLight ? LOW_LIGHT_EVT : NORMAL_LIGHT_EVT);
        }
    }

    if (SUCCEED(rc)) {
        FaceInfo faces;
        rc = mPal->getParm(faces);
        if (SUCCEED(rc) && faces.size() > 0) {
            FaceInfo::Face face = faces.get(0);
            sendEvtCallback(EEVT_PORTRAIT_BLINK,
                face.blink ? PORTRAIT_EYE_BLINK_EVT : PORTRAIT_EYE_OPEN_EVT);
        }
        RESETRESULT(rc);
    }
#if 0
    if (SUCCEED(rc)) {
        mAlgStatus[ALG_IMAGE_STABILIZATION].enable =
            checkImageStabOn();

        if (mAlgStatus[ALG_IMAGE_STABILIZATION].enable) {
            AEInfo info;
            rc = mPal->getParm(info);
            if (SUCCEED(rc)) {
                AlgTraits<PhotoSolid>::ParmType parm;
                parm.lux_index = info.lux_index;
                parm.iso_value = info.iso_value;
                parm.cameraid  = 0;

                CameraTypeInf cam;
                rc = mPal->getParm(cam);
                if (cam.type == CameraTypeInf::CAMERA_TYPE_BACK_0) {
                    parm.cameraid = 0;
                } else if (cam.type == CameraTypeInf::CAMERA_TYPE_FRONT_0) {
                    parm.cameraid = 1;
                }
                sp<IAlgorithm> alg = getAlgorithm(ALG_IMAGE_STABILIZATION);
                if (NOTNULL(alg)) {
                    rc = alg->config(parm);
                    if (!SUCCEED(rc)) {
                        LOGE(mModule, "Failed to config %s, %d",
                            alg->whoamI(), rc);
                    }
                }
            }
            RESETRESULT(rc);
        }
    }

    if (SUCCEED(rc)) {
        if (mAlgStatus[ALG_HDR_CHECKER].enable) {
            AEInfo info;
            rc = mPal->getParm(info);
            if (SUCCEED(rc)) {
                AlgTraits<HDRChecker>::ParmType parm;
                parm.lux_index    = info.lux_index;
                parm.luma_target  = info.luma_target;
                parm.hdr_force_on = mCamStatus.HdrStatus == HDR_MODE_FORCED_ON;
                parm.camera_id    = 0;

                CameraTypeInf camid;
                rc = mPal->getParm(camid);
                if (SUCCEED(rc)) {
                    if (camid.isFrontCamera()) {
                        parm.camera_id = 1;
                    } else if (camid.isSubCamera()) {
                        parm.camera_id = 2;
                    }
                }

                sp<IAlgorithm> alg = getAlgorithm(ALG_HDR_CHECKER);
                if (NOTNULL(alg)) {
                    rc = alg->config(parm);
                    if (!SUCCEED(rc)) {
                        LOGE(mModule, "Failed to config %s, %d",
                            alg->whoamI(), rc);
                    }
                }
            }
            RESETRESULT(rc);
        }
    }

    if (SUCCEED(rc)) {
        if (mAlgStatus[ALG_RT_BOKEH].enable) {
            AlgTraits<RTBokeh>::ParmType parm;
            AFInfo info;
            rc = mPal->getParm(info);
            if (SUCCEED(rc)) {
                if ((info.state == AFInfo::AF_STATE_FOCUSED_LOCKED) ||
                    (info.state == AFInfo::AF_STATE_PASSIVE_FOCUSED)) {
                    parm.afState = 1;
                } else {
                    parm.afState = 0;
                }
            }

            SubCamData sub;
            rc  = mPal->getParm(sub);
            if (SUCCEED(rc)) {
                if (sub.roiMapW > 0 && sub.roiMapH > 0) {
                    parm.roiMapW = sub.roiMapW;
                    parm.roiMapH = sub.roiMapH;
                } else {
                    parm.roiMapW = 0;
                    parm.roiMapH = 0;
                }
            }

            parm.previewFocusX = 0;
            parm.previewFocusY = 0;
            parm.calibSize = 0;

            sp<IAlgorithm> alg = getAlgorithm(ALG_RT_BOKEH);
            if (NOTNULL(alg)) {
                rc = alg->config(parm);
                if (!SUCCEED(rc)) {
                    LOGE(mModule, "Failed to config %s, %d",
                        alg->whoamI(), rc);
                }
            }
            RESETRESULT(rc);
        }
    }

    if (SUCCEED(rc)) {
        if (mAlgStatus[ALG_STILL_BOKEHA].enable) {
            AlgTraits<StillBokehA>::ParmType parm;
            AEInfo ae;
            rc = mPal->getParm(ae);
            if (SUCCEED(rc)) {
                parm.parmType = AlgTraits<StillBokehA>::
                    ParmType::STILL_BOKEH_A_PARM_TYPE_3A;
                parm.iso = ae.iso_value;
            }

            FaceInfo faceInfo;
            rc = mPal->getParm(faceInfo);
            if (SUCCEED(rc) && faceInfo.size() > 0) {
                PreviewDim preivewSize;
                PictureDim pictureSize;
                rc  = mPal->getParm(preivewSize);
                rc |= mPal->getParm(pictureSize);
                if (SUCCEED(rc)) {
                    int32_t faceLeft = faceInfo.get(0).rect.left;
                    int32_t faceTop = faceInfo.get(0).rect.top;
                    int32_t faceW = faceInfo.get(0).rect.width;
                    int32_t faceH = faceInfo.get(0).rect.height;
                    int32_t picW = pictureSize.w;
                    int32_t picH = pictureSize.h;
                    int32_t prvW = preivewSize.w;
                    int32_t prvH = preivewSize.h;
                    parm.faceFocusX = (int32_t)(
                        (faceLeft + faceW / 2) * ((float)picW / prvW));
                    parm.faceFocusY = (int32_t)(
                        (faceTop + faceH / 2) * ((float)picH / prvH));
                } else {
                    parm.faceFocusX = 0;
                    parm.faceFocusY = 0;
                }
            }

            if (SUCCEED(rc)) {
                sp<IAlgorithm> alg = getAlgorithm(ALG_STILL_BOKEHA);
                if (NOTNULL(alg)) {
                    rc = alg->config(parm);
                    if (!SUCCEED(rc)) {
                        LOGE(mModule, "Failed to config %s, %d",
                            alg->whoamI(), rc);
                    }
                }
            }
            RESETRESULT(rc);
        }
    }

    if (SUCCEED(rc)) {
        if (mAlgStatus[ALG_DUAL_CAM_NIGHT_SHOT].enable) {
            AlgTraits<DualCamNightShot>::ParmType parm;
            AEInfo ae;
            rc = mPal->getParm(ae);
            if (SUCCEED(rc)) {
                parm.parmType = AlgTraits<DualCamNightShot>::
                    ParmType::DUAL_NIGHT_PARM_TYPE_3A;
                parm.iso = ae.iso_value;
                sp<IAlgorithm> alg = getAlgorithm(ALG_DUAL_CAM_NIGHT_SHOT);
                if (NOTNULL(alg)) {
                    rc = alg->config(parm);
                    if (!SUCCEED(rc)) {
                        LOGE(mModule, "Failed to config %s, %d",
                            alg->whoamI(), rc);
                    }
                }
            }
            RESETRESULT(rc);
        }
    }

    if (SUCCEED(rc)) {
        AEInfo ae;
        rc = mPal->getParm(ae);
        if (SUCCEED(rc)) {
            sp<IAlgorithm> alg1 = getAlgorithm(ALG_BEAUTY_FACE);
            sp<IAlgorithm> alg2 = getAlgorithm(ALG_BEAUTY_FACE_CAP);
            if (NOTNULL(alg1) && NOTNULL(alg1)) {
                AlgTraits<BeautyFace>::ParmType parm;
                parm.camType.type = CameraTypeInf::CAMERA_TYPE_MAX_INVALID;
                parm.rotation     = ROTATION_ANGLE_MAX_INVALID;
                parm.manual       = -1;
                parm.luxIndex     = ae.lux_index;
                parm.backlight    = checkHdrOn();
                parm.age          = -1;
                parm.gender       = GENDER_TYPE_MAX_INVALID;
                rc = alg1->config(parm);
                if (!SUCCEED(rc)) {
                    LOGE(mModule, "Failed to config %s, %d",
                        alg1->whoamI(), rc);
                }
                rc = alg2->config(parm);
                if (!SUCCEED(rc)) {
                    LOGE(mModule, "Failed to config %s, %d",
                        alg2->whoamI(), rc);
                }
            }
        }
        RESETRESULT(rc);
    }

    if (SUCCEED(rc)) {
        if (mAlgStatus[ALG_SUPER_RESOLUTION].enable) {
            AEInfo ae;
            rc = mPal->getParm(ae);
            if (SUCCEED(rc)) {
                sp<IAlgorithm> alg = getAlgorithm(ALG_SUPER_RESOLUTION);
                if (NOTNULL(alg)) {
                    AlgTraits<SuperResolution>::ParmType parm;
                    parm.zoomRatio = -1.0f;
                    parm.iso       = ae.iso_value;
                    parm.expTime   = ae.exp_time;
                    rc = alg->config(parm);
                    if (!SUCCEED(rc)) {
                        LOGE(mModule, "Failed to config %s, %d",
                            alg->whoamI(), rc);
                    }
                }
            }
            RESETRESULT(rc);
        }
    }

    if (SUCCEED(rc)) {
        if (mAlgStatus[ALG_PICTURE_ZOOM2].enable) {
            AEInfo ae;
            rc = mPal->getParm(ae);
            if (SUCCEED(rc)) {
                sp<IAlgorithm> alg = getAlgorithm(ALG_PICTURE_ZOOM2);
                if (NOTNULL(alg)) {
                    AlgTraits<PictureZoom2>::ParmType parm;
                    parm.zoomRatio = -1.0f;
                    parm.iso       = ae.iso_value;
                    parm.expTime   = ae.exp_time;
                    rc = alg->config(parm);
                    if (!SUCCEED(rc)) {
                        LOGE(mModule, "Failed to config %s, %d", alg->whoamI(), rc);
                    }
                }
            }
        }
        RESETRESULT(rc);
    }
#endif
    return RETURNIGNORE(rc, NOT_INITED);
}

int32_t PandoraCore::updateCommand(ReqArgs<TT_COMMAND_AVAILABLE> &task)
{
    int32_t rc = NO_ERROR;
    CommandInf cmd;

    LOGD(mModule, "Command %d %d %d reveived",
        task.command, task.arg1, task.arg2);

    if (SUCCEED(rc)) {
        rc = mPal->onCommandAvailable(task.command, task.arg1, task.arg2);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to notify pal about new command");
        }

        if (SUCCEED(rc)) {
            rc = mPal->getParm(cmd);
            if (!SUCCEED(rc)) {
                LOGD(mModule, "Failed to get cmd from pal");
            }
        }
    }
#if 0
    if (SUCCEED(rc)) {
        switch (cmd.type) {
            case CommandInf::COMMAND_TYPE_LONGSHOT_ON: {
                mCamStatus.longshot = true;
                memcpy(mAlgStatusBackup, mAlgStatus, sizeof(mAlgStatusBackup));
                for (int32_t i = 0; i < ALG_MAX_INVALID; i++) {
                    mAlgStatus[i].enable = mAlgStatus[i].enabled = false;
                }

                if (mAlgStatus[ALG_SMART_SELECT].enable) {
                    AlgTraits<SmartSelect>::ParmType parm;
                    parm.mStatus = AlgTraits<SmartSelect>::
                        ParmType::SMART_SELECT_STATUS_START;
                    rc = getAlgorithm(ALG_SMART_SELECT)->config(parm);
                    if (!SUCCEED(rc)) {
                        LOGE(mModule, "Failed to config %s, %d",
                            getAlgorithm(ALG_SMART_SELECT)->whoamI(), rc);
                    }
                }
            } break;
            case CommandInf::COMMAND_TYPE_LONGSHOT_OFF: {
                if (mCamStatus.longshot) {
                    memcpy(mAlgStatus, mAlgStatusBackup, sizeof(mAlgStatus));
                }
                mCamStatus.longshot = false;
                if (mAlgStatus[ALG_SMART_SELECT].enable) {
                    AlgTraits<SmartSelect>::ParmType parm;
                    parm.mStatus = AlgTraits<SmartSelect>::
                        ParmType::SMART_SELECT_STATUS_STOP;
                    rc = getAlgorithm(ALG_SMART_SELECT)->config(parm);
                    if (!SUCCEED(rc)) {
                        LOGE(mModule, "Failed to config %s, %d",
                            getAlgorithm(ALG_SMART_SELECT)->whoamI(), rc);
                    }

                    TaskType algTask;
                    algTask.data = NULL;
                    algTask.size = algTask.w = algTask.h = 0;
                    rc = sendTaskToAlgorithm(ALG_SMART_SELECT, algTask);
                }
            } break;
            default: {
                // Extend command here
            } break;
        }
    }
#endif
    return rc;
}

int32_t PandoraCore::updateParameter(
    ReqArgs<TT_PARAMETER_AVAILABLE> &task)
{
    int32_t rc = NO_ERROR;
    LOGD(mModule, "Update parameter %p", task.parm);

    if (SUCCEED(rc)) {
        if (ISNULL(task.parm)) {
            LOGE(mModule, "Invalid parm");
//            rc = NOT_INITED;
        }
    }

    if (SUCCEED(rc)) {
        rc = mPal->onParameterAvailable(task.parm);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to notify pal about new parm");
        }
    }

    if (SUCCEED(rc)) {
        PreviewDim previewSize;
        rc = mPal->getParm(previewSize);
        if (SUCCEED(rc) && previewSize != mCamStatus.previewSize) {
            onPreviewSizeChanged(mCamStatus.previewSize, previewSize);
            mCamStatus.previewSize = previewSize;
        }

        PictureDim pictureSize;
        rc = mPal->getParm(pictureSize);
        if (SUCCEED(rc) && pictureSize != mCamStatus.pictureSize) {
            onPictureSizeChanged(mCamStatus.pictureSize, pictureSize);
            mCamStatus.pictureSize = pictureSize;
        }

        VideoDim videoSize;
        mCamStatus.videoSize.w = mCamStatus.videoSize.h = 0;
        rc = mPal->getParm(videoSize);
        if (SUCCEED(rc) && videoSize != mCamStatus.videoSize) {
            onVideoSizeChanged(mCamStatus.videoSize, videoSize);
            mCamStatus.videoSize = videoSize;
        }
        RESETRESULT(rc);
    }


    if (SUCCEED(rc)) {
        BeautySetting beauty;
        rc = mPal->getParm(beauty);
        if (!SUCCEED(rc)) {
            LOGD(mModule, "beauty setting not set or invalid");
        } else {
            switch(beauty.mode) {
#if 0
            case BEAUTY_MODE_ON:
            case BEAUTY_MODE_MANUAL: {
                mAlgStatus[ALG_BEAUTY_FACE].enable = true;
                mAlgStatus[ALG_BEAUTY_FACE_CAP].enable = true;
                mAlgStatus[ALG_AGE_GENDER_DETECTION].enable = true;
                mAlgStatus[ALG_MICRO_PLASTIC].enable = true;
                sp<IAlgorithm> alg1 = getAlgorithm(ALG_BEAUTY_FACE);
                sp<IAlgorithm> alg2 = getAlgorithm(ALG_BEAUTY_FACE_CAP);
                if (NOTNULL(alg1) && NOTNULL(alg1)) {
                    AlgTraits<BeautyFace>::ParmType parm;

                    CameraTypeInf camid;
                    RotationAngle rotation;
                    rc = mPal->getParm(camid);
                    rc |= mPal->getParm(rotation);
                    if (SUCCEED(rc)) {
                        parm.camType  = camid;
                        parm.rotation = rotation;
                    }
                    parm.manual = beauty.mode == BEAUTY_MODE_MANUAL ? beauty.strength : 0;
                    parm.luxIndex  = -1;
                    parm.backlight = -1;
                    parm.age       = -1;
                    parm.gender    = GENDER_TYPE_MAX_INVALID;
                    rc = alg1->config(parm);
                    if (!SUCCEED(rc)) {
                        LOGE(mModule, "Failed to config %s, %d", alg1->whoamI(), rc);
                    }
                    rc = alg2->config(parm);
                    if (!SUCCEED(rc)) {
                        LOGE(mModule, "Failed to config %s, %d", alg2->whoamI(), rc);
                    }
                    mAlgStatus[ALG_BEAUTY_FACE].enabled = true;
                    mAlgStatus[ALG_BEAUTY_FACE_CAP].enabled = true;
                } else {
                    LOGE(mModule, "Failed to create beauty face algorithm");
                    NOTNULL(alg1) ? removeAlgorithm(ALG_BEAUTY_FACE) : NO_ERROR;
                    NOTNULL(alg2) ? removeAlgorithm(ALG_BEAUTY_FACE_CAP) : NO_ERROR;
                }

                sp<IAlgorithm> alg3 = getAlgorithm(ALG_AGE_GENDER_DETECTION);
                if (NOTNULL(alg3)) {
                    mAlgStatus[ALG_AGE_GENDER_DETECTION].enabled = true;
                } else {
                    NOTNULL(alg2) ? removeAlgorithm(ALG_AGE_GENDER_DETECTION) : NO_ERROR;
                }

                sp<IAlgorithm> alg4 = getAlgorithm(ALG_MICRO_PLASTIC);
                if (NOTNULL(alg4)) {
                    mAlgStatus[ALG_MICRO_PLASTIC].enabled = true;
                } else {
                    NOTNULL(alg2) ? removeAlgorithm(ALG_MICRO_PLASTIC) : NO_ERROR;
                }
            } break;
#endif
            case BEAUTY_MODE_OFF: {
                mAlgStatus[ALG_BEAUTY_FACE].enable  = false;
                mAlgStatus[ALG_BEAUTY_FACE].enabled = false;
                mAlgStatus[ALG_BEAUTY_FACE_CAP].enable  = false;
                mAlgStatus[ALG_BEAUTY_FACE_CAP].enabled = false;
                mAlgStatus[ALG_AGE_GENDER_DETECTION].enable  = false;
                mAlgStatus[ALG_AGE_GENDER_DETECTION].enabled = false;
                mAlgStatus[ALG_MICRO_PLASTIC].enable  = false;
                mAlgStatus[ALG_MICRO_PLASTIC].enabled = false;
            } break;
            case BEAUTY_MODE_UNDEFINED:
            case BEAUTY_MODE_MAX_INVALID:
            default :
                break;
            }
        }
        RESETRESULT(rc);
    }
#if 0
    if (SUCCEED(rc)) {
        PlatformPriv platformPriv;
        rc = mPal->getParm(platformPriv);
        if (SUCCEED(rc)) {
            mCamStatus.platformPriv = platformPriv;
            if (platformPriv.scaleDim.w && platformPriv.scaleDim.h) {
                sp<IAlgorithm> alg1 = getAlgorithm(ALG_PICTURE_ZOOM);
                if (NOTNULL(alg1)) {
                    AlgTraits<PictureZoom>::ParmType parm;
                    parm.mWidth = platformPriv.scaleDim.w;
                    parm.mHeight= platformPriv.scaleDim.h;
                    rc = alg1->config(parm);
                    if (!SUCCEED(rc)) {
                        LOGE(mModule, "Failed to config %s, %d",
                            alg1->whoamI(), rc);
                    }
                    mAlgStatus[ALG_PICTURE_ZOOM].enable = true;
                }
            } else {
                mAlgStatus[ALG_PICTURE_ZOOM].enable = false;
            }
        }
        RESETRESULT(rc);
    }

    if (SUCCEED(rc)) {
        ZoomInf inf;
        rc = mPal->getParm(inf);
        if (SUCCEED(rc)) {
            if (checkZoomOn(inf.ratio)) {
                sp<IAlgorithm> alg = getAlgorithm(ALG_PICTURE_ZOOM2);
                if (NOTNULL(alg)) {
                    AlgTraits<PictureZoom2>::ParmType parm;
                    parm.zoomRatio = inf.ratio;
                    parm.iso       = -1;
                    parm.expTime   = -1;
                    rc = alg->config(parm);
                    if (!SUCCEED(rc)) {
                        LOGE(mModule, "Failed to config %s, %d", alg->whoamI(), rc);
                    }
                    mAlgStatus[ALG_PICTURE_ZOOM2].enable = true;
                }
            } else {
                mAlgStatus[ALG_PICTURE_ZOOM2].enable = false;
            }
        }
        RESETRESULT(rc);
    }

    if (SUCCEED(rc)) {
        FairLightMode mode;
        rc = mPal->getParm(mode);
        if (SUCCEED(rc)) {
            if (mode != FAIRLIGHT_NONE) {
                FaceInfo faces;
                rc = mPal->getParm(faces);
                if (SUCCEED(rc)) {
                    sp<IAlgorithm> alg = getAlgorithm(ALG_FAIR_LIGHT);
                    if (NOTNULL(alg)) {
                        AlgTraits<FairLight>::ParmType parm;
                        parm.rotation = faces.get(0).roll_dir;
                        parm.mode     = mode;
                        rc = alg->config(parm);
                        if (!SUCCEED(rc)) {
                            LOGE(mModule, "Failed to config %s, %d", alg->whoamI(), rc);
                        }
                        mAlgStatus[ALG_FAIR_LIGHT].enable = true;
                    } else {
                        mAlgStatus[ALG_FAIR_LIGHT].enable = false;
                    }
                }
            }
        }
        RESETRESULT(rc);
    }

    if (SUCCEED(rc)) {
        RotationAngle rotation;
        rc = mPal->getParm(rotation);
        if (SUCCEED(rc)) {
            sp<IAlgorithm> alg = getAlgorithm(ALG_MICRO_PLASTIC);
            if (NOTNULL(alg)) {
                AlgTraits<MicroPlastic>::ParmType parm;
                parm.rotation = rotation;
                rc = alg->config(parm);
                if (!SUCCEED(rc)) {
                    LOGE(mModule, "Failed to config %s, %d", alg->whoamI(), rc);
                }
            }
        }
        RESETRESULT(rc);
    }

    if (SUCCEED(rc)) {
        HdrMode mode;
        rc = mPal->getParm(mode);
        if (!SUCCEED(rc)) {
            LOGD(mModule, "hdr setting not set or invalid");
        } else {
            mCamStatus.HdrStatus = mode;
            mAlgStatus[ALG_HDR_CHECKER].enable =
                mode != HDR_MODE_FORCED_OFF;
            if (mAlgStatus[ALG_HDR_CHECKER].enable) {
                sp<IAlgorithm> alg = getAlgorithm(ALG_HDR_CHECKER);
                if (NOTNULL(alg)) {
                    mAlgStatus[ALG_HDR_CHECKER].enabled = true;
                } else {
                    LOGE(mModule, "Failed to create hdr checker algorithm");
                }
            } else {
                mAlgStatus[ALG_HDR_CHECKER].enabled = false;
                mAlgStatus[ALG_HDR].enable  = false;
                mAlgStatus[ALG_HDR].enabled = false;
            }

            if (mode == HDR_MODE_FORCED_ON && mMultiExposure.size() == 0) {
                int32_t defaultExp[] = { -6, 0, 6 };
                RWLock::AutoWLock l(mMultiExposureLock);
                mMultiExposure.clear();

                AlgTraits<HDR>::ParmType parm;
                parm.expNum   = ARRAYSIZE(defaultExp);
                parm.cameraid = 0;
                CameraTypeInf camid;
                rc = mPal->getParm(camid);
                if (SUCCEED(rc)) {
                    if (camid.isFrontCamera()) {
                        parm.cameraid = 1;
                    } else if (camid.isSubCamera()) {
                        parm.cameraid = 2;
                    }
                }
                for (uint32_t i = 0; i < ARRAYSIZE(defaultExp); i++) {
                    parm.exp[i] = defaultExp[i];
                    mMultiExposure.add(defaultExp[i]);
                }
                sp<IAlgorithm> alg = getAlgorithm(ALG_HDR);
                if (NOTNULL(alg)) {
                    rc = alg->config(parm);
                    if (!SUCCEED(rc)) {
                        LOGE(mModule, "Failed to config %s, %d",
                            alg->whoamI(), rc);
                    } else {
                        mAlgStatus[ALG_HDR].enable = true;
                    }
                }
            }

            if (mode == HDR_MODE_FORCED_ON) {
                sendEvtCallback(EEVT_HDR_STATUS, HDR_ON_EVT);
            } else {
                sendEvtCallback(EEVT_HDR_STATUS, HDR_OFF_EVT);
            }
        }
        RESETRESULT(rc);
    }
#endif
    if (SUCCEED(rc)) {
        FlashMode mode;
        rc = mPal->getParm(mode);
        if (!SUCCEED(rc)) {
            LOGD(mModule, "flash setting not set or invalid");
            RESETRESULT(rc);
        } else {
            mCamStatus.flashMode = mode;
            if (mode == FLASH_MODE_FORCED_ON) {
                sendEvtCallback(EEVT_FLASH_STATUS, FLASH_ON_EVT);
            } else {
                // Follow 3A output, don't send off event
                //sendEvtCallback(EEVT_FLASH_STATUS, FLASH_OFF_EVT);
            }
        }
    }

    if (SUCCEED(rc)) {
        LongshotMode mode;
        rc = mPal->getParm(mode);
        if (!SUCCEED(rc)) {
            LOGD(mModule, "long shot setting not set or invalid");
            RESETRESULT(rc);
        } else {
            mCamStatus.longshot = mode == LONG_SHOT_MODE_ON;
        }
    }

    if (SUCCEED(rc)) {
        LongExpMode mode;
        rc = mPal->getParm(mode);
        if (!SUCCEED(rc)) {
            LOGD(mModule, "long exposure setting not set or invalid");
            RESETRESULT(rc);
        } else {
            mCamStatus.longexposure = mode.on == true;
        }
    }

    if (SUCCEED(rc)) {
        SmartshotMode mode;
        rc = mPal->getParm(mode);
        if (!SUCCEED(rc)) {
            LOGD(mModule, "smart shot setting not set or invalid");
            RESETRESULT(rc);
        } else {
            mAlgStatus[ALG_SMART_SHOT].enable =
                mode == SMART_SHOT_MODE_ON;
        }
    }

    if (SUCCEED(rc)) {
        VideoStabMode mode;
        rc = mPal->getParm(mode);
        if (!SUCCEED(rc)) {
            LOGD(mModule, "video stabilizer setting not set or invalid");
        } else {
            mCamStatus.VSMode = mode;
            if (mode == VIDEO_STAB_MODE_PREVIEW_ONLY ||
                mode == VIDEO_STAB_MODE_PREVIEW_VIDEO) {
                mAlgStatus[ALG_VIDEO_STAB_PREVIEW].enable = true;
                sp<IAlgorithm> alg = getAlgorithm(ALG_VIDEO_STAB_PREVIEW);
                if (NOTNULL(alg)) {
                    mAlgStatus[ALG_VIDEO_STAB_PREVIEW].enabled = true;
                } else {
                    LOGE(mModule, "Failed to create video stab preview algorithm");
                }
            } else {
                mAlgStatus[ALG_VIDEO_STAB_PREVIEW].enable  = false;
                mAlgStatus[ALG_VIDEO_STAB_PREVIEW].enabled = false;
            }
            if (mode == VIDEO_STAB_MODE_VIDEO_ONLY ||
                mode == VIDEO_STAB_MODE_PREVIEW_VIDEO) {
                mAlgStatus[ALG_VIDEO_STAB_RECORDING].enable = true;
                sp<IAlgorithm> alg = getAlgorithm(ALG_VIDEO_STAB_RECORDING);
                if (NOTNULL(alg)) {
                    mAlgStatus[ALG_VIDEO_STAB_RECORDING].enabled = true;
                } else {
                    LOGE(mModule, "Failed to create video stab video algorithm");
                }
            } else {
                mAlgStatus[ALG_VIDEO_STAB_RECORDING].enable  = false;
                mAlgStatus[ALG_VIDEO_STAB_RECORDING].enabled = false;
            }
        }
        RESETRESULT(rc);
    }

    if (SUCCEED(rc)) {
        FlipMode mode;
        rc = mPal->getParm(mode);
        if (!SUCCEED(rc)) {
            LOGD(mModule, "flip setting not set or invalid");
            RESETRESULT(rc);
        } else {
            mCamStatus.flipMode = mode;
        }
    }

    if (SUCCEED(rc)) {
        DualCamMode mode;
        rc = mPal->getParm(mode);
        if (!SUCCEED(rc)) {
            LOGD(mModule, "Dual camera mode not set or invalid");
        } else {
            mCamStatus.dualCamMode = mode;
            mAlgStatus[ALG_STILL_BOKEH].enable  = mode.enableStillBokeh();
            mAlgStatus[ALG_RT_BOKEH].enable     = mode.enableRTBokeh();
            mAlgStatus[ALG_STILL_BOKEHA].enable = mode.enableRefocus();
            mAlgStatus[ALG_DUAL_CAM_NIGHT_SHOT].enable = mode.enableNightshot();

            if (!mAlgStatus[ALG_STILL_BOKEH].enable) {
                mAlgStatus[ALG_STILL_BOKEH].enabled = false;
            }
            if (!mAlgStatus[ALG_STILL_BOKEHA].enable) {
                mAlgStatus[ALG_STILL_BOKEHA].enabled = false;
            }
            if (!mAlgStatus[ALG_DUAL_CAM_NIGHT_SHOT].enable) {
                mAlgStatus[ALG_DUAL_CAM_NIGHT_SHOT].enabled = false;
            }
            if (mAlgStatus[ALG_RT_BOKEH].enable) {
                sp<IAlgorithm> alg = getAlgorithm(ALG_RT_BOKEH);
                if (NOTNULL(alg)) {
                    mAlgStatus[ALG_RT_BOKEH].enabled = true;
                } else {
                    LOGE(mModule, "Failed to create RT Bokeh algorithm");
                }
            } else {
                mAlgStatus[ALG_RT_BOKEH].enabled = false;
            }
            if (ISNULL(mFrameReorder)) {
                mFrameReorder = FrameReorder::getInstance();
                if (ISNULL(mFrameReorder)) {
                    LOGE(mModule, "Failed to get frame reorder.");
                    rc = INTERNAL_ERROR;
                }
            }
        }
        RESETRESULT(rc);
    }

    if (SUCCEED(rc)) {
        RotationAngle rotation;
        rc = mPal->getParm(rotation);
        if (!SUCCEED(rc)) {
            LOGD(mModule, "Rotation angle not set or invalid");
            RESETRESULT(rc);
        } else {
            mCamStatus.rotation = rotation;
        }
    }

    if (SUCCEED(rc)) {
        SubCamData  sub;
        DualCamMode mode;
        rc |= mPal->getParm(sub);
        rc |= mPal->getParm(mode);
        if (!SUCCEED(rc)) {
            LOGD(mModule, "Sub camera data not set or invalid");
        } else {
#if 0
            if (mode.enableRTBokeh() &&
                NOTNULL(sub.mainOTP) && sub.mainOTPSize) {
                AlgTraits<RTBokeh>::ParmType preview_parm;
                sp<IAlgorithm> algPreview = getAlgorithm(ALG_RT_BOKEH);
                if (NOTNULL(algPreview)) {
                    preview_parm.calibSize = sub.mainOTPSize;
                    memcpy(preview_parm.calibData, sub.mainOTP, sub.mainOTPSize);
                    preview_parm.previewFocusX = sub.previewFocusX;
                    preview_parm.previewFocusY = sub.previewFocusY;
                    preview_parm.roiMapW = sub.roiMapW;
                    preview_parm.roiMapH = sub.roiMapH;

                    AFInfo info;
                    rc = mPal->getParm(info);
                    if (SUCCEED(rc)) {
                        if ((info.state == AFInfo::AF_STATE_FOCUSED_LOCKED) ||
                            (info.state == AFInfo::AF_STATE_PASSIVE_FOCUSED)) {
                            preview_parm.afState = 1;
                        } else {
                            preview_parm.afState = 0;
                        }
                    }

                    rc = algPreview->config(preview_parm);
                    if (!SUCCEED(rc)) {
                        LOGE(mModule, "Failed to config %s, %d", algPreview->whoamI(), rc);
                    }
                    RESETRESULT(rc);
                }
            }

            if (mode.enableRefocus() &&
                NOTNULL(sub.mainOTP) && sub.mainOTPSize) {
                AlgTraits<StillBokehA>::ParmType picture_parm;
                sp<IAlgorithm> algPicture = getAlgorithm(ALG_STILL_BOKEHA);
                if (NOTNULL(algPicture)) {
                    picture_parm.parmType =
                        AlgTraits<StillBokehA>::ParmType::STILL_BOKEH_A_PARM_TYPE_SETTINGS;
                    picture_parm.calibSize = sub.mainOTPSize;
                    memcpy(picture_parm.calibData, sub.mainOTP, sub.mainOTPSize);
                    picture_parm.focusX = sub.focusX;
                    picture_parm.focusY = sub.focusY;
                    picture_parm.roiMapW = sub.roiMapW;
                    picture_parm.roiMapH = sub.roiMapH;
                    picture_parm.blurLevel = sub.blurLevel;
                    std::function<int32_t (int32_t, uint8_t *, size_t, void  (*)(void *))> func =
                        [this](int32_t t, uint8_t *d, size_t s, void  (*r)(void *)) -> int32_t {
                        DataCallbackInf dat = {
                            .type = t,
                            .size = s,
                            .data = d,
                            .release = r,
                        };
                        int32_t rc = mPal->dataNotify(dat, SYNC_TYPE);
                        if (!SUCCEED(rc)) {
                            LOGE(mModule, "Failed to notify app for data type %d", t);
                        }
                        return rc;
                    };
                    picture_parm.mCbFunc = &func;
                    rc = algPicture->config(picture_parm);
                    if (!SUCCEED(rc)) {
                        LOGE(mModule, "Failed to config %s, %d", algPicture->whoamI(), rc);
                    }
                }
            }

            if (mode.enableNightshot() &&
                NOTNULL(sub.mainOTP) && sub.mainOTPSize) {
                AlgTraits<DualCamNightShot>::ParmType nightshot_parm;
                sp<IAlgorithm> algDCNightshot = getAlgorithm(ALG_DUAL_CAM_NIGHT_SHOT);
                if (NOTNULL(algDCNightshot)) {
                    nightshot_parm.parmType =
                        AlgTraits<DualCamNightShot>::ParmType::DUAL_NIGHT_PARM_TYPE_SETTINGS;
                    nightshot_parm.calibSize = sub.mainOTPSize;
                    memcpy(nightshot_parm.calibData, sub.mainOTP, sub.mainOTPSize);

                    rc = algDCNightshot->config(nightshot_parm);
                    if (!SUCCEED(rc)) {
                        LOGE(mModule, "Failed to config %s, %d",
                            algDCNightshot->whoamI(), rc);
                    }
                }
            }

            if (mode.enableStillBokeh()) {
                AlgTraits<StillBokeh>::ParmType parm;
                if (NOTNULL(sub.mainOTP) && sub.mainOTPSize) {
                    parm.calibSize = sub.mainOTPSize;
                    memcpy(parm.calibData, sub.mainOTP, sub.mainOTPSize);
                }
                if (NOTNULL(sub.subOTP) && sub.subOTPSize) {
                    memcpy(parm.calibData + parm.calibSize, sub.subOTP, sub.subOTPSize);
                    parm.calibSize += sub.subOTPSize;
                }
                parm.mainVCMDAC = sub.mainVCMDAC;
                parm.subVCMDAC = sub.subVCMDAC;
                parm.blurLevel = sub.blurLevel;
                parm.focusX = sub.focusX;
                parm.focusY = sub.focusY;

                AEInfo ae;
                rc = mPal->getParm(ae);
                if (SUCCEED(rc)) {
                    parm.exposureTime = ae.exp_time;
                    parm.iso = ae.iso_value;
                } else {
                    parm.exposureTime = 1.0f / 30;
                    parm.iso = 100;
                    RESETRESULT(rc);
                }

                std::function<int32_t (int32_t, uint8_t *, size_t, void  (*)(void *))> func =
                    [this](int32_t t, uint8_t *d, size_t s, void  (*r)(void *)) -> int32_t {
                    DataCallbackInf dat = {
                        .type = t,
                        .size = s,
                        .data = d,
                        .release = r,
                    };

                    int32_t rc = mPal->dataNotify(dat, SYNC_TYPE);
                    if (!SUCCEED(rc)) {
                        LOGE(mModule, "Failed to notify app for data type %d", t);
                    }
                    return rc;
                };
                parm.mCbFunc = &func;

                sp<IAlgorithm> alg = getAlgorithm(ALG_STILL_BOKEH);
                if (NOTNULL(alg)) {
                    rc = alg->config(parm);
                    if (!SUCCEED(rc)) {
                        LOGE(mModule, "Failed to config %s, %d", alg->whoamI(), rc);
                    }
                }
            }
#endif
        }
        RESETRESULT(rc);
    }
#if 0
    if (SUCCEED(rc)) {
        ZoomInf inf;
        rc = mPal->getParm(inf);
        if (SUCCEED(rc)) {
            if (checkZoomOn(inf.ratio)) {
                sp<IAlgorithm> alg = getAlgorithm(ALG_SUPER_RESOLUTION);
                if (NOTNULL(alg)) {
                    AlgTraits<SuperResolution>::ParmType parm;
                    parm.zoomRatio = inf.ratio;
                    parm.iso       = -1;
                    parm.expTime   = -1;
                    rc = alg->config(parm);
                    if (!SUCCEED(rc)) {
                        LOGE(mModule, "Failed to config %s, %d", alg->whoamI(), rc);
                    }
                    mAlgStatus[ALG_SUPER_RESOLUTION].enable = true;
                }
            } else {
                mAlgStatus[ALG_SUPER_RESOLUTION].enable  = false;
                mAlgStatus[ALG_SUPER_RESOLUTION].enabled = false;
            }
        }
        RESETRESULT(rc);
    }

    if (SUCCEED(rc)) {
        SingleBokehInf bokeh;
        rc = mPal->getParm(bokeh);
        if (SUCCEED(rc)) {
            if (bokeh.mode == SINGLE_CAM_BOKEH_ON) {
                sp<IAlgorithm> alg1 = getAlgorithm(ALG_SINGLE_BOKEH);
                sp<IAlgorithm> alg2 = getAlgorithm(ALG_SINGLE_BOKEH_CAP);
                if (NOTNULL(alg1) && NOTNULL(alg1)) {
                    AlgTraits<SingleBokeh>::ParmType parm;
                    parm.blurLevel = bokeh.blurLevel;
                    parm.rotation = mCamStatus.rotation;
                    rc = alg1->config(parm);
                    if (!SUCCEED(rc)) {
                        LOGE(mModule, "Failed to config %s, %d", alg1->whoamI(), rc);
                    }
                    rc = alg2->config(parm);
                    if (!SUCCEED(rc)) {
                        LOGE(mModule, "Failed to config %s, %d", alg2->whoamI(), rc);
                    }
                    mAlgStatus[ALG_SINGLE_BOKEH].enable = true;
                    mAlgStatus[ALG_SINGLE_BOKEH_CAP].enable = true;
                }
            } else {
                mAlgStatus[ALG_SINGLE_BOKEH].enable  = false;
                mAlgStatus[ALG_SINGLE_BOKEH].enabled = false;
                mAlgStatus[ALG_SINGLE_BOKEH_CAP].enable  = false;
                mAlgStatus[ALG_SINGLE_BOKEH_CAP].enabled = false;
            }
        }
        RESETRESULT(rc);
    }
#endif
    return rc;
}

int32_t PandoraCore::processPreviewAlgs(TaskType &task)
{
    int32_t rc = NO_ERROR;

    if (mAlgStatus[ALG_BEAUTY_FACE].enabled && !checkRecording()) {
        rc = sendTaskToAlgorithm(ALG_BEAUTY_FACE, task);
    }

    if (mAlgStatus[ALG_MICRO_PLASTIC].enabled && !checkRecording()) {
        rc = sendTaskToAlgorithm(ALG_MICRO_PLASTIC, task);
    }

    if (mAlgStatus[ALG_VIDEO_STAB_PREVIEW].enabled) {
        rc = sendTaskToAlgorithm(ALG_VIDEO_STAB_PREVIEW, task);
    }

    if (mAlgStatus[ALG_SINGLE_BOKEH].enabled && !checkRecording()) {
        rc = sendTaskToAlgorithm(ALG_SINGLE_BOKEH, task);
    }

    if (mAlgStatus[ALG_RT_BOKEH].enabled) {
        rc |= ISNULL(mFrameReorder) ? NO_ERROR :
            mFrameReorder->wait(task.type, task.format);
        rc |= sendTaskToAlgorithm(ALG_RT_BOKEH, task);
        rc |= ISNULL(mFrameReorder) ? NO_ERROR :
            mFrameReorder->reorder(task.type, task.format);
    }

    if (mAlgStatus[ALG_HDR_CHECKER].enabled ||
        mAlgStatus[ALG_AGE_GENDER_DETECTION].enabled) {
        rc = mBufferedPreview.replace(0, task);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to buffer preview frame, %d", rc);
        }
    }

    if (mAlgStatus[ALG_HDR_CHECKER].enabled) {
        sp<IAlgorithm> alg = getAlgorithm(ALG_HDR_CHECKER);
        if (NOTNULL(alg) && !alg->busy()) {
            TaskType _task = task;
            _task.data = mBufferedPreview[0];
            rc = sendTaskToAlgorithm(
                ALG_HDR_CHECKER, _task, ASYNC_TYPE);
        }
    }
#if 0
    if (mAlgStatus[ALG_AGE_GENDER_DETECTION].enabled) {
        sp<IAlgorithm> alg = getAlgorithm(ALG_AGE_GENDER_DETECTION);
        if (NOTNULL(alg) && !alg->busy()) {
            AlgTraits<AgeGenderDetection>::TaskType _task(task);
            _task.data    = mBufferedPreview[0];
            _task.request = this;
            rc = sendTaskToAlgorithm(
                ALG_AGE_GENDER_DETECTION, _task, ASYNC_TYPE);
        }
    }
#endif
    return rc;
}

int32_t PandoraCore::processVideoAlgs(TaskType &task)
{
    int32_t rc = NO_ERROR;

    if (mAlgStatus[ALG_VIDEO_STAB_RECORDING].enabled) {
        rc = sendTaskToAlgorithm(ALG_VIDEO_STAB_RECORDING, task);
    }

    return rc;
}

int32_t PandoraCore::processSnapshotAlgs(TaskType &task)
{
    int32_t rc = NO_ERROR;
    bool isTarget = false;

    mCamStatus.snapshotCnt++;

    if (SUCCEED(rc)) {
        if (mAlgStatus[ALG_SMART_SHOT].enabled) {
            rc |= sendTaskToAlgorithm(ALG_SMART_SHOT, task);
        }

        if (mAlgStatus[ALG_ANTI_SHAKING].enabled) {
            rc |= sendTaskToAlgorithm(ALG_ANTI_SHAKING, task);
        }

        if (mAlgStatus[ALG_NIGHT_SHOT].enabled) {
            rc |= sendTaskToAlgorithm(ALG_NIGHT_SHOT, task);
        }

        if (mAlgStatus[ALG_SMART_SELECT].enabled && mCamStatus.longshot) {
            rc |= sendTaskToAlgorithm(ALG_SMART_SELECT, task);
        }

        if (mAlgStatus[ALG_HDR].enabled) {
            rc |= sendTaskToAlgorithm(ALG_HDR, task);
        }

        if (mAlgStatus[ALG_IMAGE_STABILIZATION].enabled) {
            rc |= sendTaskToAlgorithm(ALG_IMAGE_STABILIZATION, task);
        }

        if (mAlgStatus[ALG_PICTURE_ZOOM].enabled) {
            rc |= sendTaskToAlgorithm(ALG_PICTURE_ZOOM, task);
        }

        if (mAlgStatus[ALG_PICTURE_ZOOM2].enabled) {
            rc |= sendTaskToAlgorithm(ALG_PICTURE_ZOOM2, task);
        }

        if (mAlgStatus[ALG_SUPER_RESOLUTION].enabled) {
            rc |= sendTaskToAlgorithm(ALG_SUPER_RESOLUTION, task);
        }
    }

    if (SUCCEED(rc)) {
        if (task.type == FRAME_TYPE_SNAPSHOT &&
            task.format == FRAME_FORMAT_JPEG) {
            rc = mExifReader->process(task);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to process SW jpeg encoder, %d", rc);
            }
        }
    }

    if (SUCCEED(rc)) {
        rc = mFrameCounter->countFrame(
            task.type, task.format, &isTarget);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to count frame, %d", rc);
        } else {
            if (!isTarget) {
                rc = JUMP_DONE;
            }
        }
    }

    if (SUCCEED(rc)) {
        if (mAlgStatus[ALG_BEAUTY_FACE_CAP].enabled) {
            rc |= sendTaskToAlgorithm(ALG_BEAUTY_FACE_CAP, task);
        }

        if (mAlgStatus[ALG_SINGLE_BOKEH_CAP].enabled) {
            rc |= sendTaskToAlgorithm(ALG_SINGLE_BOKEH_CAP, task);
        }
    }

    if (SUCCEED(rc)) {
        rc = drawWaterMarkEncodeJpeg(task);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to draw wm, encode, send data, %d", rc);
        }
    }

    return RETURNIGNORE(rc, JUMP_DONE);
}

int32_t PandoraCore::processDualCamAlgs(TaskType &task)
{
    int32_t rc = NO_ERROR;
    bool isTarget = false;

    if (NOTNULL(mFrameReorder)) {
        rc = mFrameReorder->wait(task.type, task.format);
    }

    if (mAlgStatus[ALG_STILL_BOKEH].enabled) {
        rc |= sendTaskToAlgorithm(ALG_STILL_BOKEH, task);
    }

    if (mAlgStatus[ALG_STILL_BOKEHA].enabled) {
        rc |= sendTaskToAlgorithm(ALG_STILL_BOKEHA, task);
    }

    if (mAlgStatus[ALG_DUAL_CAM_NIGHT_SHOT].enabled) {
        rc |= sendTaskToAlgorithm(ALG_DUAL_CAM_NIGHT_SHOT, task);
    }

    if (NOTNULL(mFrameReorder)) {
        rc |= mFrameReorder->reorder(task.type, task.format);
    }

    if (SUCCEED(rc)) {
        if (task.type == FRAME_TYPE_SNAPSHOT &&
            task.format == FRAME_FORMAT_JPEG) {
            rc = mExifReader->process(task);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to process SW jpeg encoder, %d", rc);
            }
        }
    }

    if (SUCCEED(rc)) {
        rc = mFrameCounter->countFrame(
            task.type, task.format, &isTarget);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to count frame, %d", rc);
        } else {
            if (!isTarget) {
                rc = JUMP_DONE;
            }
        }
    }

    if (SUCCEED(rc)) {
        rc = drawWaterMarkEncodeJpeg(task);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to draw wm, encode, send data, %d", rc);
        }
    }

    return RETURNIGNORE(rc, JUMP_DONE);
}

int32_t PandoraCore::drawWaterMark(TaskType &task)
{
    int32_t rc = NO_ERROR;
    WaterMarkInfo info;

    if (SUCCEED(rc)) {
        rc = mPal->getParm(info);
        if (SUCCEED(rc)) {
            if (info.mode == WATERMARK_MODE_OFF) {
                LOGD(mModule, "Not needed to draw water mark.");
                rc = NOT_NEEDED;
            }
        } else {
            LOGD(mModule, "Unknown watermark setting, "
                "ignore and disable, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        if (ISNULL(mWaterMark)) {
            mWaterMark = Algorithm<WaterMark>::create(
                "WaterMark", "WaterMarkThread", false, 1, 0);
            if (ISNULL(mWaterMark)) {
                LOGE(mModule, "Failed to create watermark algorithm");
                rc = INTERNAL_ERROR;
            }
        }
    }

    if (SUCCEED(rc)) {
        AlgCaps cap = mWaterMark->queryCap();
        if (!(cap.types & getFrameTypeMask(task.type) &&
            cap.formats & getFrameFormatMask(task.format))) {
            LOGD(mModule, "Algorithm %s not support such frame "
                "type %s format %s", mWaterMark->getName(),
                frameTypeName(task.type), frameFormatName(task.format));
            rc = NOT_NEEDED;
        }
    }

    if (SUCCEED(rc)) {
        AlgTraits<WaterMark>::ParmType parm;
        parm.rotation = mCamStatus.rotation;
        parm.info = info;
        rc = mWaterMark->config(parm);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to config water mark, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        if (NOTNULL(mWaterMark)) {
            rc = mWaterMark->process(task);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to draw watermark, %d", rc);
            }
        }
    }

    return RETURNIGNORE(rc, NOT_NEEDED);
}

int32_t PandoraCore::encodeJpegSW(TaskType &task, JpegHeader *jpeg)
{
    int32_t rc = 0;
    int32_t jpegSize = 0;

    if (SUCCEED(rc)) {
        if (ISNULL(mJpegEncoder)) {
            mJpegEncoder = Algorithm<JpegSWEncoder>::create(
                "JpegEncoder", "JpegEncoderThread", false, 1, 0);
            if (ISNULL(mJpegEncoder)) {
                LOGE(mModule, "Failed to create JpegEncoder algorithm");
                rc = INTERNAL_ERROR;
            }
        }
    }

    if (SUCCEED(rc)) {
        AlgTraits<JpegSWEncoder>::TaskType _task = task;
        _task.exif = mExifReader;
        AlgTraits<JpegSWEncoder>::TaskType _jpeg = task;
        _jpeg.data = (uint8_t *)jpeg + sizeof(JpegHeader);
        rc = mJpegEncoder->process(_task, _jpeg);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to process SW jpeg encoder, %d", rc);
        }
        jpegSize = _jpeg.size;
    }

    if (SUCCEED(rc)) {
        jpeg->w = task.w;
        jpeg->h = task.h;
        jpeg->size = jpegSize;
    }

    return rc;
}

int32_t PandoraCore::drawWaterMarkEncodeJpeg(TaskType &task)
{
    int32_t rc = NO_ERROR;
    BufHolder<Global> jpegBuf;
    JpegHeader *header = NULL;
    DataCallbackInf dat;

    if (SUCCEED(rc)) {
        rc = drawWaterMark(task);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to draw water mark, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = jpegBuf.replace(0, task);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to create jpeg buf, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        header = (JpegHeader *)jpegBuf[0];
        rc = encodeJpegSW(task, header);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to encode jpeg by sw, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        dat.type = 0x0DBA,
        dat.size = sizeof(JpegHeader) + header->size,
        dat.data = jpegBuf[0],
        dat.release = NULL,
        rc = mPal->dataNotify(dat, SYNC_TYPE);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to notify app for data type %d", rc);
        }
    }


    return rc;
}

int32_t PandoraCore::bufferAsyncSnapshotFrame(TaskType &task, void **newBuf)
{
    int32_t rc = NO_ERROR;
    *newBuf    = NULL;
    bool dualCam = mCamStatus.dualCamMode.enabledAny();

    if (task.type != FRAME_TYPE_SNAPSHOT) {
        rc = NOT_NEEDED;
    }

    if (SUCCEED(rc)) {
        if (!mFrameCounter->getCounter()) {
            mBufferedPair.clear();
            mBufferedSnapshot.clear();
        }
    }

    if (SUCCEED(rc)) {
        rc = dualCam ?
            mBufferedPair.add(task) :
            mBufferedSnapshot.add(task);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to add snapshot buffer, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        TaskType frame;
        rc = !dualCam ?
            mBufferedSnapshot.getLastTask(frame) :
            mBufferedPair.getLastTask(task.format, frame);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to get buffer frame, %d", rc);
        } else {
            *newBuf = frame.data;
        }
    }

    return RETURNIGNORE(rc, NOT_NEEDED);
}

int32_t PandoraCore::onframeReadyAsync(TaskType &task)
{
    int32_t rc = NO_ERROR;

    if (task.type == FRAME_TYPE_PREVIEW) {
        rc = processPreviewAlgs(task);
    }

    if (task.type == FRAME_TYPE_VIDEO) {
        rc = processVideoAlgs(task);
    }

    if (task.type == FRAME_TYPE_SNAPSHOT ||
        mCamStatus.dualCamMode.enabledAny()) {
        void *newBuf = NULL;
        rc = bufferAsyncSnapshotFrame(task, &newBuf);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to buffer frame, %d", rc);
        } else {
            task.data = newBuf;
        }
    }

    if (mCamStatus.dualCamMode.enabledAny()) {
        rc = mThreads->run(
            [this, task]() -> int32_t {
                int32_t rc = NO_ERROR;
                TaskType _task = task;
                rc = processDualCamAlgs(_task);
                if (!SUCCEED(rc)) {
                    LOGE(mModule, "Failed to process "
                        "dual camera algorithms", rc);
                }
                return rc;
            }
        );
    }

    if (task.type == FRAME_TYPE_SNAPSHOT &&
        mCamStatus.dualCamMode.enabledNone()) {
        rc = mThreads->run(
            [this, task]() -> int32_t {
                int32_t rc = NO_ERROR;
                TaskType _task = task;
                rc = processSnapshotAlgs(_task);
                if (!SUCCEED(rc)) {
                    LOGE(mModule, "Failed to process "
                        "snapshot algorithms", rc);
                }
                return rc;
            }
        );
    }

    return rc;
}

int32_t runAlgNewIntfTester(TaskType &task);

int32_t PandoraCore::onframeReadySync(TaskType &task)
{
    int32_t rc = NO_ERROR;
    bool isTargetFrame = false;

    if (task.type == FRAME_TYPE_PREVIEW) {
        rc = processPreviewAlgs(task);
    }

    if (task.type == FRAME_TYPE_VIDEO) {
        rc = processVideoAlgs(task);
    }

    if (mCamStatus.dualCamMode.enabledAny()) {
        if (NOTNULL(mFrameReorder)) {
            rc = mFrameReorder->wait(task.type, task.format);
        }

        if (mAlgStatus[ALG_RT_BOKEH].enabled) {
            rc |= sendTaskToAlgorithm(ALG_RT_BOKEH, task);
        }

        if (mAlgStatus[ALG_STILL_BOKEH].enabled) {
            rc |= sendTaskToAlgorithm(ALG_STILL_BOKEH, task);
        }

        if (mAlgStatus[ALG_STILL_BOKEHA].enabled) {
            rc |= sendTaskToAlgorithm(ALG_STILL_BOKEHA, task);
        }

        if (mAlgStatus[ALG_DUAL_CAM_NIGHT_SHOT].enabled) {
            rc |= sendTaskToAlgorithm(ALG_DUAL_CAM_NIGHT_SHOT, task);
        }

        if (NOTNULL(mFrameReorder)) {
            rc |= mFrameReorder->reorder(task.type, task.format);
        }
    }

    if (task.type == FRAME_TYPE_SNAPSHOT) {
        mCamStatus.snapshotCnt++;
        if (mAlgStatus[ALG_SMART_SHOT].enabled) {
            rc = sendTaskToAlgorithm(ALG_SMART_SHOT, task);
        }
        if (mAlgStatus[ALG_ANTI_SHAKING].enabled) {
            rc = sendTaskToAlgorithm(ALG_ANTI_SHAKING, task);
        }
        if (mAlgStatus[ALG_NIGHT_SHOT].enabled) {
            rc = sendTaskToAlgorithm(ALG_NIGHT_SHOT, task);
        }
        if (mAlgStatus[ALG_SMART_SELECT].enabled && mCamStatus.longshot) {
            rc = sendTaskToAlgorithm(ALG_SMART_SELECT, task);
        }
        if (mAlgStatus[ALG_HDR].enabled) {
            rc = sendTaskToAlgorithm(ALG_HDR, task);
        }
        if (mAlgStatus[ALG_IMAGE_STABILIZATION].enabled) {
            rc = sendTaskToAlgorithm(ALG_IMAGE_STABILIZATION, task);
        }
        if (mAlgStatus[ALG_PICTURE_ZOOM].enabled) {
            rc = sendTaskToAlgorithm(ALG_PICTURE_ZOOM, task);
        }
        if (mAlgStatus[ALG_FAIR_LIGHT].enabled) {
            rc = sendTaskToAlgorithm(ALG_FAIR_LIGHT, task);
        }
        if (mAlgStatus[ALG_MICRO_PLASTIC].enabled) {
            rc = sendTaskToAlgorithm(ALG_MICRO_PLASTIC, task);
        }
        if (mAlgStatus[ALG_PICTURE_ZOOM2].enabled) {
            rc = sendTaskToAlgorithm(ALG_PICTURE_ZOOM2, task);
        }
        if (mAlgStatus[ALG_SUPER_RESOLUTION].enabled) {
            rc = sendTaskToAlgorithm(ALG_SUPER_RESOLUTION, task);
        }
        if (mAlgStatus[ALG_BEAUTY_FACE_CAP].enabled &&
            (mCamStatus.dualCamMode.enabledAny() ||
            mCamStatus.snapshotCnt == mCamStatus.snapshotNeeded)) {
            rc = sendTaskToAlgorithm(ALG_BEAUTY_FACE_CAP, task);
        }
        if (mAlgStatus[ALG_SINGLE_BOKEH_CAP].enabled) {
            rc = sendTaskToAlgorithm(ALG_SINGLE_BOKEH_CAP, task);
        }

        if (mTestAlgIntf) {
            rc = runAlgNewIntfTester(task);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to test alg new intf, %d", rc);
            }
        }
    }

    if (!SUCCEED(rc)) {
        LOGE(mModule, "Failed to process frame this time, %d", rc);
        LOGI(mModule, "Frame dump: Task id %d, "
            "%p %dx%d stride %d scanline %d "
            "size %d format %d type %d timestamp %lld",
            task.taskid, task.data, task.w, task.h, task.stride,
            task.scanline, task.size, task.format, task.type, task.ts);

        RESETRESULT(rc);
    }

    if (SUCCEED(rc)) {
        rc = mFrameCounter->countFrame(task.type, task.format, &isTargetFrame);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to count frame, %d", rc);
        }
    }
    isTargetFrame = 1;
    if (SUCCEED(rc)) {
        if (task.type == FRAME_TYPE_SNAPSHOT && isTargetFrame) {
            rc = drawWaterMark(task);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to draw water mark, %d", rc);
                RESETRESULT(rc);
            }
        }
    }

    return rc;
}

int32_t PandoraCore::onframeReady(ReqArgs<TT_FRAME_READY> &t)
{
    int32_t rc = NO_ERROR;

    TaskType task(t.frame.frame,
        t.frame.w, t.frame.h, t.frame.stride,
        t.frame.scanline, t.frame.size,
        t.frame.format, t.frame.type, t.frame.ts);
    if (mAsyncSnapshot) {
        rc = onframeReadyAsync(task);
    } else {
        rc = onframeReadySync(task);
    }

    if (!SUCCEED(rc)) {
        LOGE(mModule, "Failed to process frame, %d"
            "id %d, frame %p %dx%d stride %d scanline %d "
            "size %d format %d type %d timestamp %lld",
            rc, task.taskid, task.data, task.w, task.h, task.stride,
            task.scanline, task.size, task.format, task.type, task.ts);
    }


    return rc;
}

int32_t PandoraCore::sendEvtCallback(ExtendedCBEvt cbevt, int32_t arg)
{
    int32_t rc = NO_ERROR;

    if (ISNULL(mPal)) {
        rc = NOT_INITED;
    }

    if (SUCCEED(rc)) {
        if (mExtEvtId == -1) {
            rc = mPal->getParm(mExtEvtId);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to get extended evt id, %d", rc);
            }
        }
    }

    if (SUCCEED(rc)) {
        if (checkEventSent(cbevt, arg)) {
            LOGD(mModule, " Evt %s arg %d already sent, skip.",
                getEvtName(cbevt), arg);
            rc = NOT_NEEDED;
        }
    }

    if (SUCCEED(rc)) {
        EvtCallbackInf evt = {
            .evt  = mExtEvtId,
            .type = cbevt,
            .arg  = arg,
        };

        rc = mPal->evtNotify(evt);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to send extended event, rc %d", rc);
        } else {
            LOGD(mModule, " Send evt %s arg %d to framework.",
                getEvtName(cbevt), arg);
        }
    }

    if (SUCCEED(rc)) {
        rc = recordSentEvent(cbevt, arg);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to record event %s", getEvtName(cbevt));
            RESETRESULT(rc);
        }
    }


    return RETURNIGNORE(rc, NOT_NEEDED);
}

int32_t PandoraCore::onAlgResultAvailable(
    int32_t /*taskId*/, ResultType *_result)
{
    int32_t rc = NO_ERROR;
    AlgType type = getValidType(_result->type);

    if (!mAlgStatus[type].enabled) {
        // Algorithm is removing, ignore results
        LOGD(mModule, "Algorithm %s is been removing, "
            "ignore result.", getAlgName(type));
        rc = JUMP_DONE;
    }

    if (SUCCEED(rc)) {
        switch (type) {
            case ALG_BEAUTY_FACE:
            case ALG_BEAUTY_FACE_CAP:
            case ALG_HDR:
            case ALG_IMAGE_STABILIZATION:
            case ALG_SUPER_RESOLUTION:
            case ALG_PICTURE_ZOOM:
            case ALG_FAIR_LIGHT:
            case ALG_MICRO_PLASTIC:
            case ALG_PICTURE_ZOOM2:
            case ALG_VIDEO_STAB_PREVIEW:
            case ALG_VIDEO_STAB_RECORDING:
            case ALG_STILL_BOKEH:
            case ALG_STILL_BOKEHA:
            case ALG_RT_BOKEH:
            case ALG_DUAL_CAM_NIGHT_SHOT:
            case ALG_SINGLE_BOKEH:
            case ALG_SINGLE_BOKEH_CAP:
            case ALG_UTILS: {
                // Do not care...
            } break;
#if 0
            case ALG_AGE_GENDER_DETECTION: {
                AlgTraits<AgeGenderDetection>::ResultType *result = static_cast<
                    AlgTraits<AgeGenderDetection>::ResultType *>(_result);
                if (result->valid) {
                    sp<IAlgorithm> alg1 = getAlgorithm(ALG_BEAUTY_FACE);
                    sp<IAlgorithm> alg2 = getAlgorithm(ALG_BEAUTY_FACE_CAP);
                    if (NOTNULL(alg1) && NOTNULL(alg1)) {
                        AlgTraits<BeautyFace>::ParmType parm;
                        parm.camType.type = CameraTypeInf::CAMERA_TYPE_MAX_INVALID;
                        parm.rotation     = ROTATION_ANGLE_MAX_INVALID;
                        parm.manual       = -1;
                        parm.luxIndex     = -1;
                        parm.backlight    = -1;
                        parm.age          = result->age;
                        parm.gender       = result->gender;
                        rc = alg1->config(parm);
                        if (!SUCCEED(rc)) {
                            LOGE(mModule, "Failed to config %s, %d", alg1->whoamI(), rc);
                        }
                        rc = alg2->config(parm);
                        if (!SUCCEED(rc)) {
                            LOGE(mModule, "Failed to config %s, %d", alg2->whoamI(), rc);
                        }
                    }
                }
            } break;
            case ALG_HDR_CHECKER: {
                AlgTraits<HDRChecker>::ResultType *result = static_cast<
                    AlgTraits<HDRChecker>::ResultType *>(_result);

                if (result->valid && !mCamStatus.takingPic) {
                    HdrInfo info;
                    if (result->num == 1) {
                       info.is_hdr_scene = 0;
                       info.hdr_confidence = 0.0f;
                    } else {
                       info.is_hdr_scene = 1;
                       info.hdr_confidence = 1.0f;
                    }
                    mStatusMgr.push_back(info);

                    bool required = false;
                    if (checkHdrOn()) {
                        sendEvtCallback(EEVT_HDR_STATUS, HDR_ON_EVT);
                        required = true;
                    } else {
                        sendEvtCallback(EEVT_HDR_STATUS, HDR_OFF_EVT);
                    }

                    if (result->num == 1) {
                        HdrMode mode;
                        rc = mPal->getParm(mode);
                        if (SUCCEED(rc)) {
                            if (mode == HDR_MODE_FORCED_ON) {
                                int32_t defaultExp[] = { -6, 0, 6 };
                                result->num = ARRAYSIZE(defaultExp);
                                for (int32_t i = 0; i < result->num; i++) {
                                    result->exp[i] = defaultExp[i];
                                }
                                required = true;
                            }
                        }
                    }

                    mAlgStatus[ALG_HDR].enable = false;
                    if (required) {
                        RWLock::AutoWLock l(mMultiExposureLock);
                        mMultiExposure.clear();
                        AlgTraits<HDR>::ParmType parm;
                        parm.expNum   = result->num;
                        parm.cameraid = 0;
                        for (int32_t i = 0; i < result->num; i++) {
                            parm.exp[i] = result->exp[i];
                            mMultiExposure.add(result->exp[i]);
                        }

                        CameraTypeInf camid;
                        rc = mPal->getParm(camid);
                        if (SUCCEED(rc)) {
                            if (camid.isFrontCamera()) {
                                parm.cameraid = 1;
                            } else if (camid.isSubCamera()) {
                                parm.cameraid = 2;
                            }
                        }

                        sp<IAlgorithm> alg = getAlgorithm(ALG_HDR);
                        if (NOTNULL(alg)) {
                            rc = alg->config(parm);
                            if (SUCCEED(rc)) {
                                mAlgStatus[ALG_HDR].enable = true;
                            } else {
                                LOGE(mModule, "Failed to config %s, %d",
                                    alg->whoamI(), rc);
                            }
                        }
                    }

                    if (!mAlgStatus[ALG_HDR].enable) {
                        mAlgStatus[ALG_HDR].enabled = false;
                    }
                }
            } break;
            case ALG_SMART_SELECT: {
                AlgTraits<SmartSelect>::ResultType *result = static_cast<
                    AlgTraits<SmartSelect>::ResultType *>(_result);
                if (result->valid) {
                    sendEvtCallback(EEVT_SMART_SELECT, result->index);
                }
            } break;
#endif
            case ALG_MAX_INVALID:
            default: {
                LOGE(mModule, "Invalid algorithm type %d", _result->type);
                rc = UNKNOWN_ERROR;
            } break;
        }
    }

    return RETURNIGNORE(rc, JUMP_DONE);
}

int32_t PandoraCore::onAlgRequest(RequestType *request, AlgType submitter)
{
    int32_t rc = NO_ERROR;
    AlgType type = getValidType(submitter);

    if (!mAlgStatus[type].enabled) {
        // Algorithm is removing, ignore request
        LOGD(mModule, "Algorithm %s is been removing, "
            "ignore request.", getAlgName(type));
        rc = JUMP_DONE;
    }

    if (SUCCEED(rc)) {
        switch (type) {
            case ALG_BEAUTY_FACE:
            case ALG_BEAUTY_FACE_CAP:
            case ALG_HDR:
            case ALG_IMAGE_STABILIZATION:
            case ALG_SUPER_RESOLUTION:
            case ALG_PICTURE_ZOOM:
            case ALG_FAIR_LIGHT:
            case ALG_MICRO_PLASTIC:
            case ALG_PICTURE_ZOOM2:
            case ALG_VIDEO_STAB_PREVIEW:
            case ALG_VIDEO_STAB_RECORDING:
            case ALG_STILL_BOKEH:
            case ALG_STILL_BOKEHA:
            case ALG_RT_BOKEH:
            case ALG_DUAL_CAM_NIGHT_SHOT:
            case ALG_SINGLE_BOKEH:
            case ALG_SINGLE_BOKEH_CAP:
            case ALG_UTILS:
            case ALG_HDR_CHECKER:
            case ALG_SMART_SELECT:
                // Nothing to do
                break;
#if 0
            case ALG_AGE_GENDER_DETECTION: {
                AlgTraits<AgeGenderDetection>::RequestType *info = static_cast<
                    AlgTraits<AgeGenderDetection>::RequestType *>(request);
                rc = mPal->getParm(info->faces);
                if (!SUCCEED(rc)) {
                    LOGE(mModule, "Failed to get faces info, %d", rc);
                }
            } break;
#endif
            case ALG_MAX_INVALID:
            default: {
                LOGE(mModule, "Invalid algorithm type %d", submitter);
                rc = UNKNOWN_ERROR;
            } break;
        }
    }

    return RETURNIGNORE(rc, JUMP_DONE);
}

PandoraCore::AlgorithmInfo *PandoraCore::getAlgorithm(
    sp<IAlgorithm> alg)
{
    AlgorithmInfo *result = NULL;

    RWLock::AutoRLock l(mAlgLock);
    for (auto &info : mAlgorithms) {
        if (info.alg == alg) {
            result = &info;
            break;
        }
    }

    return result;
}

int32_t PandoraCore::removeAlgorithm(AlgType type)
{
    bool found = false;
    int32_t rc = NO_ERROR;

    if (getValidType(type) == ALG_MAX_INVALID) {
        LOGE(mModule, "Invalid algorithm type %d", type);
        rc = INTERNAL_ERROR;
    } else {
        mAlgStatus[type].enabled = false;
    }

    if (SUCCEED(rc)) {
        RWLock::AutoWLock l(mAlgLock);
        auto iter = mAlgorithms.begin();
        for (; iter != mAlgorithms.end(); iter++) {
            if (iter->alg->getType() == type) {
                found = true;
                break;
            }
        }
        if (found) {
            mAlgMask &= ~(getAlgMask(type));
            mAlgorithms.erase(iter);
        } else {
            rc = NOT_INITED;
        }
    }

    return rc;
}

int32_t PandoraCore::sendTaskToAlgorithm(AlgType type, TaskType &task, SyncTypeE sync)
{
    int32_t rc = NO_ERROR;
    sp<IAlgorithm> alg = NULL;
    AlgorithmInfo *inf = NULL;

    if (SUCCEED(rc)) {
        alg = getAlgorithm(type);
        if (ISNULL(alg)) {
            LOGE(mModule, "Failed to get algorithm, %s", getAlgName(type));
            rc = INTERNAL_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        inf = getAlgorithm(alg);
        if (ISNULL(inf)) {
            LOGE(mModule, "Failed to get algorithm info.");
            rc = INTERNAL_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        if (!(inf->caps.types & getFrameTypeMask(task.type) &&
            inf->caps.formats & getFrameFormatMask(task.format))) {
            LOGD(mModule, "Algorithm %s not support such frame "
                "type %s format %s", inf->name,
                frameTypeName(task.type), frameFormatName(task.format));
            rc = NOT_NEEDED;
        }
    }

    if (SUCCEED(rc)) {
        rc = alg->process(task, task.taskid, sync);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to add new task to algorithm %s", getAlgName(type));
        }
    }

    return RETURNIGNORE(rc, NOT_NEEDED);
}

bool PandoraCore::checkFrontCamera()
{
    int32_t rc = NO_ERROR;
    bool result = false;

    CameraTypeInf camera;
    rc = mPal->getParm(camera);
    if (!SUCCEED(rc)) {
        LOGE(mModule, "Failed to get camera type from pal");
    } else {
        result = camera.isFrontCamera();
    }

    return result;
}

bool PandoraCore::checkFlashOn()
{
    return mCamStatus.flashMode == FLASH_MODE_FORCED_ON ||
        (mCamStatus.flashMode == FLASH_MODE_AUTO &&
            mStatusMgr.checkAutoFlashOn());
}

bool PandoraCore::checkHdrOn()
{
    return !checkRecording() &&
        !mCamStatus.longshot &&
        !mCamStatus.longexposure &&
        !mCamStatus.dualCamMode.enabledAny() &&
        !mCamStatus.dualCamMode.enableExternal() &&
        !checkFlashOn() &&
        (mCamStatus.HdrStatus == HDR_MODE_FORCED_ON || (
             mCamStatus.HdrStatus == HDR_MODE_AUTO &&
                mStatusMgr.checkAutoHdrOn()));
}

bool PandoraCore::checkAutoHdrOn()
{
    return !checkRecording() &&
        !checkFlashOn() &&
        (mCamStatus.HdrStatus == HDR_MODE_AUTO &&
            mStatusMgr.checkAutoHdrOn());
}

bool PandoraCore::checkImageStabMode()
{
    int32_t rc = NO_ERROR;
    bool result = false;

    ImageStabMode isMode;
    rc = mPal->getParm(isMode);
    if (SUCCEED(rc) && isMode == IMAGE_STAB_MODE_ON) {
        result = true;
    }

    return result;
}

bool PandoraCore::checkImageStabOn()
{
    return !checkRecording() &&
        !mCamStatus.longshot &&
        !mCamStatus.longexposure &&
        !mCamStatus.dualCamMode.enabledAny() &&
        !mCamStatus.dualCamMode.enableExternal() &&
        !checkFlashOn() &&
        !checkHdrOn() &&
        (checkImageStabMode() ||
            mStatusMgr.checkAutoImageStabOn());
}

bool PandoraCore::checkZoomOn(float zoomRatio)
{
    return zoomRatio > 1.0f &&
        !checkRecording() &&
        !mCamStatus.longshot &&
        !mCamStatus.longexposure &&
        !mCamStatus.dualCamMode.enabledAny() &&
        !mCamStatus.dualCamMode.enableExternal() &&
        !checkFlashOn() &&
        !checkHdrOn() &&
        !checkImageStabOn();
}

bool PandoraCore::checkHdrChanged()
{
    return !checkRecording() &&
        mCamStatus.HdrStatus == HDR_MODE_AUTO &&
        mStatusMgr.checkAutoHDRChanged();
}

bool PandoraCore::checkFlashChanged()
{
    return !checkRecording() &&
        mStatusMgr.checkFlashChanged();
}

bool PandoraCore::checkAutoFlashOn()
{
    return !checkRecording() &&
        mCamStatus.flashMode == FLASH_MODE_AUTO &&
        mStatusMgr.checkAutoFlashOn();
}

int32_t PandoraCore::recordSentEvent(ExtendedCBEvt evt, int32_t arg)
{
    int32_t rc = NO_ERROR;

    if (!isEvtValid(evt)) {
        LOGE(mModule, "Invalid evt %d, maximum %d", evt, EEVT_MAX_INVALID);
        rc = UNKNOWN_ERROR;
    }

    if (SUCCEED(rc)) {
        mEvents[evt].evt  = mExtEvtId;
        mEvents[evt].type = evt;
        mEvents[evt].arg  = arg;
    }

    return rc;
}

bool PandoraCore::checkEventSent(ExtendedCBEvt evt, int32_t arg)
{
    int32_t rc = NO_ERROR;
    bool result = false;

    static const ExtendedCBEvt skipCheckEvts[] = {
        EEVT_FOCUS_CHANGED,
        EEVT_LUMINANCE_CHANGED,
        EEVT_SMART_SELECT,
        EEVT_ALG_PROCESSING,
        EEVT_ALG_FINISHED,
    };

    if (!isEvtValid(evt)) {
        LOGE(mModule, "Invalid evt %d, maximum %d", evt, EEVT_MAX_INVALID);
        rc = UNKNOWN_ERROR;
    }

    if (SUCCEED(rc)) {
        for (uint32_t i = 0; i < ARRAYSIZE(skipCheckEvts); i++) {
            if (evt == skipCheckEvts[i]) {
                rc = NOT_NEEDED;
                break;
            }
        }
    }

    if (SUCCEED(rc)) {
        if (mEvents[evt].evt  == mExtEvtId &&
            mEvents[evt].type == evt &&
            mEvents[evt].arg  == arg) {
            result = true;
        }
    }

    return result;
}

static const char * const gEvtName[] = {
    [EEVT_FOCUS_CHANGED]     = "EEVT_FOCUS_CHANGED",
    [EEVT_LUMINANCE_CHANGED] = "EEVT_LUMINANCE_CHANGED",
    [EEVT_SMART_SELECT]      = "EEVT_SMART_SELECT",
    [EEVT_ALG_PROCESSING]    = "EEVT_ALG_PROCESSING",
    [EEVT_ALG_FINISHED]      = "EEVT_ALG_FINISHED",
    [EEVT_PORTRAIT_BLINK]    = "EEVT_PORTRAIT_BLINK",
    [EEVT_FLASH_STATUS]      = "EEVT_FLASH_STATUS",
    [EEVT_HDR_STATUS]        = "EEVT_HDR_STATUS",
    [EEVT_IMAG_STAB_STATUS]  = "EEVT_IMAG_STAB_STATUS",
    [EEVT_LIGHT_STATUS]      = "EEVT_LIGHT_STATUS",
    [EEVT_MAX_INVALID]       = "EEVT_MAX_INVALID",
};

const char *getEvtName(ExtendedCBEvt evt)
{
    const char *result = "INVALID_EVT";

    if (evt >= EEVT_FOCUS_CHANGED && evt <= EEVT_MAX_INVALID) {
        result = gEvtName[evt];
    }

    return result;
}

bool isEvtValid(ExtendedCBEvt evt)
{
    bool rc = false;

    if (evt >= 0 && evt < EEVT_MAX_INVALID) {
        rc = true;
    }

    return rc;
}

}; // end of namespace pandora

#define __GOTO_END_OF_THIS_FILE_

