#ifndef _PANDORA_CORE__
#define _PANDORA_CORE__

#include "Common.h"
#include "CameraStatusMgr.h"
#include "PandoraImplHelper.h"
#include "AlgorithmIntf.h"
#include "AlgorithmProperties.h"
#include "SyncType.h"
#include "Pal.h"
#include "ConfigInterface.h"
#include "PandoraParameters.h"
#include "BufHolder.h"
#include "FramePairQueue.h"
#include "RefBase.h"

namespace pandora {

class PlatformOpsIntf;
class FrameReorder;
class FrameCounter;
class PalTester;
class ThreadPoolEx;
class SiriusClient;

class PandoraCore :
    public IAlgListener,
    public IAlgRequestHandler,
    virtual public noncopyable {

public:
    int32_t startPreview();
    int32_t stopPreview();
    int32_t takePicture();
    int32_t finishPicture();
    int32_t startRecording();
    int32_t stopRecording();
    int32_t updateMetadata(ReqArgs<TT_METADATA_AVAILABLE> &task);
    int32_t updateParameter(ReqArgs<TT_PARAMETER_AVAILABLE> &task);
    int32_t updateCommand(ReqArgs<TT_COMMAND_AVAILABLE> &task);
    int32_t onframeReady(ReqArgs<TT_FRAME_READY> &task);

private:
    int32_t sendEvtCallback(ExtendedCBEvt evt, int32_t arg = 0);
    int32_t onAlgResultAvailable(int32_t taskId, ResultType *result) override;
    int32_t onAlgRequest(RequestType *request, AlgType submitter) override;
    int32_t onStartPreview();
    int32_t onStopPreview();
    int32_t onSnapshot();
    int32_t onSnapshotFinished();
    int32_t processDualCamAlgs(TaskType &task);
    int32_t processPreviewAlgs(TaskType &task);
    int32_t processSnapshotAlgs(TaskType &task);
    int32_t processVideoAlgs(TaskType &task);
    int32_t drawWaterMarkEncodeJpeg(TaskType &task);
    int32_t drawWaterMark(TaskType &task);
    struct JpegHeader;
    int32_t encodeJpegSW(TaskType &task, JpegHeader *jpeg);
    int32_t bufferAsyncSnapshotFrame(TaskType &task, void **newBuf);
    int32_t onframeReadyAsync(TaskType &task);
    int32_t onframeReadySync(TaskType &task);
    bool checkRecording();
    int32_t onPreviewSizeChanged(PreviewDim &old, PreviewDim &latest);
    int32_t onPictureSizeChanged(PictureDim &old, PictureDim &latest);
    int32_t onVideoSizeChanged(VideoDim &old, VideoDim &latest);
    bool checkFlashOn();
    bool checkHdrOn();
    bool checkAutoHdrOn();
    bool checkImageStabMode();
    bool checkImageStabOn();
    bool checkHdrChanged();
    bool checkFlashChanged();
    bool checkAutoFlashOn();
    bool checkZoomOn(float zoomRatio = 1.0f);
    int32_t UpdateAlgorithmStatus();
    bool checkFrontCamera();
    sp<IAlgorithm> getAlgorithm(AlgType type);
    struct AlgorithmInfo;
    AlgorithmInfo *getAlgorithm(sp<IAlgorithm> alg);
    int32_t sendTaskToAlgorithm(AlgType alg, TaskType &task, SyncTypeE sync = SYNC_TYPE);
    int32_t removeAlgorithm(AlgType type);
    int32_t recordSentEvent(ExtendedCBEvt evt, int32_t arg);
    bool checkEventSent(ExtendedCBEvt evt, int32_t arg);

public:
    explicit PandoraCore(PlatformOpsIntf *platform);
    ~PandoraCore();
    int32_t construct();
    int32_t destruct();

private:
    struct AlgorithmStatus {
        bool enable;
        bool enabled;
        AlgorithmStatus() :
            enable(false), enabled(false) {}
    };

    struct CameraStatus {
        bool longshot;
        bool longexposure;
        bool takingPic;
        bool recording;
        bool bracket;
        bool multishot;
        HdrMode       HdrStatus;
        FlashMode     flashMode;
        VideoStabMode VSMode;
        PreviewDim    previewSize;
        PictureDim    pictureSize;
        VideoDim      videoSize;
        PlatformPriv  platformPriv;
        FlipMode      flipMode;
        DualCamMode   dualCamMode;
        int32_t       snapshotNeeded;
        int32_t       snapshotCnt;
        RotationAngle rotation;
        CameraStatus() :
            longshot(false),
            longexposure(false),
            takingPic(false),
            recording(false),
            bracket(false),
            multishot(false),
            HdrStatus(HDR_MODE_AUTO),
            flashMode(FLASH_MODE_AUTO),
            VSMode(VIDEO_STAB_MODE_OFF),
            snapshotNeeded(1),
            snapshotCnt(0),
            rotation(ROTATION_ANGLE_0){
            dualCamMode.funcMask = 0;
        }
    };

    struct AlgorithmInfo {
        sp<IAlgorithm> alg;
        AlgType        type;
        const char    *name;
        AlgCaps        caps;
        uint32_t       frameCnt;
    };

    struct JpegHeader {
        int32_t w;
        int32_t h;
        int32_t size;
    };

private:
    bool              mConstructed;
    ModuleType        mModule;
    PlatformOpsIntf  *mPlatform;
    Pal              *mPal;
    CamStatusMgr      mStatusMgr;
    AlgorithmStatus   mAlgStatus[ALG_MAX_INVALID];
    AlgorithmStatus   mAlgStatusBackup[ALG_MAX_INVALID];
    CameraStatus      mCamStatus;
    sp<AlgProperties> mAlgProperties;
    AlgMask           mAlgMask;
    RWLock            mAlgLock;
    std::list<AlgorithmInfo> mAlgorithms;
    MultiExporeParm   mMultiExposure;
    RWLock            mMultiExposureLock;
    FrameReorder     *mFrameReorder;
    FrameCounter     *mFrameCounter;
    sp<IAlgorithm>    mWaterMark;
    sp<IAlgorithm>    mJpegEncoder;
    BufHolder<Global> mBufferedPreview;
    BufHolder<Global> mBufferedSnapshot;
    FramePairQueue<Global> mBufferedPair;
    EvtCallbackInf    mEvents[EEVT_MAX_INVALID];
    ConfigInterface::IdType  mExtEvtId;
    PalTester        *mPalTester;
    ThreadPoolEx     *mThreads;
    IAlgorithm       *mExifReader;
    bool              mAsyncSnapshot;
    bool              mTestAlgIntf;
    bool              mTestPal;
    bool              mEnableSirius;
    SiriusClient     *mSirius;
};

};

#endif
