#include "BeautyFace.h"
#include "AlgorithmHelper.h"
#include "BeautyFaceArcsoft.h"
#include "PfLogger.h"
#include "MemMgmt.h"
#include "Algorithm.h"

namespace pandora {

#define AVERAGE_PREVIEW_PERFORMANCE_COUNT  300
#define AVERAGE_SNAPSHOT_PERFORMANCE_COUNT 20

INIT_ALGORITHM_TRAITS_FUNC(BeautyFace);

BeautyFace::BeautyFace(uint32_t argNum, va_list va) :
    mInited(false),
    mPreviewPf(false),
    mSnapshotPf(false),
    mModule(MODULE_ALGORITHM),
    mPreviewPfCnt(AVERAGE_PREVIEW_PERFORMANCE_COUNT),
    mSnapshotPfCnt(AVERAGE_SNAPSHOT_PERFORMANCE_COUNT),
    mDumpPreview(false),
    mDumpSnapshot(false),
    mEngine(NULL),
    mEngineParm(NULL)
{
    mAlgType = (AlgType)va_arg(va, int32_t);
    if (mAlgType == ALG_BEAUTY_FACE ||
        mAlgType == ALG_BEAUTY_FACE_CAP) {
        LOGD(mModule, "Algorithm type %d", mAlgType);
    } else {
        LOGE(mModule, "Algorithm parm not valid, %d %d", argNum, mAlgType);
    }
}

BeautyFace::~BeautyFace()
{
    if (mInited) {
        deinit();
    }
}

int32_t BeautyFace::init()
{
    int32_t rc = NO_ERROR;

    if (mInited) {
        rc = ALREADY_EXISTS;
    }

    if (SUCCEED(rc)) {
        if (ISNULL(mEngine)) {
            mEngine = new BeautyFaceArcsoft(
                mAlgType == ALG_BEAUTY_FACE_CAP ?
                    FRAME_TYPE_SNAPSHOT : FRAME_TYPE_PREVIEW);

            if (ISNULL(mEngine)) {
                LOGE(mModule, "Failed to create beauty face engine");
                rc = NO_MEMORY;
            }
        }
    }

    if (SUCCEED(rc)) {
        rc = mEngine->init();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to init engine");
        }
    }

    if (SUCCEED(rc)) {
        mEngineParm = new BeautyFaceParm();
        if (ISNULL(mEngineParm)) {
            LOGE(mModule, "Failed to new %d bytes for parm",
                sizeof(BeautyFaceParm));
        }
    }

    if (SUCCEED(rc)) {
        rc = queryCaps(mCaps);
    }

    if (SUCCEED(rc)) {
        mInited = true;
    }

    return rc;
}

int32_t BeautyFace::deinit()
{
    int32_t rc = NO_ERROR;

    if (!mInited) {
        rc = NOT_INITED;
    }

    if (SUCCEED(rc)) {
        if (NOTNULL(mEngine)) {
            SECURE_DELETE(mEngine);
        }
        if (NOTNULL(mEngineParm)) {
            SECURE_DELETE(mEngineParm);
        }
    }

    if (SUCCEED(rc)) {
        mInited = false;
    }

    return rc;
}

int32_t BeautyFace::process(TaskTypeT &task, ResultTypeT &result)
{
    int32_t rc = NO_ERROR;
    unsigned char *frame = NULL;

    if (!mInited) {
        rc = NOT_INITED;
    } else {
        result.valid = false;
        result.taskid = task.taskid;
    }

    frame = (unsigned char *)task.data;

    if (SUCCEED(rc)) {
        if (task.format == FRAME_FORMAT_YUV_420_NV12)
            rc = convertNV12ToNV21(frame,
                task.w, task.h, task.stride, task.scanline);
    }

    if (task.type == FRAME_TYPE_PREVIEW) {
        mDumpPreview ? dumpNV21ToFile(frame, "preview_beauty_input",
            task.w, task.h, task.stride, task.scanline) : NO_ERROR;
        mPreviewPf ? PfLogger::getInstance()
            ->start("preview_beauty_process", mPreviewPfCnt) : NO_ERROR;
    } else if (task.type == FRAME_TYPE_SNAPSHOT) {
        mDumpSnapshot ? dumpNV21ToFile(frame, "snapshot_beauty_input",
            task.w, task.h, task.stride, task.scanline) : NO_ERROR;
        mSnapshotPf ? PfLogger::getInstance()
            ->start("snapshot_beauty_process", mSnapshotPfCnt) : NO_ERROR;
    }

    if (SUCCEED(rc)) {
        rc = mEngine->doProcess(frame,
            task.w, task.h, task.stride, task.scanline);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to process beauty face %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        if (task.format == FRAME_FORMAT_YUV_420_NV12)
            rc = convertNV21ToNV12(frame,
                task.w, task.h, task.stride, task.scanline);
    }

    if (task.type == FRAME_TYPE_PREVIEW) {
        mDumpPreview ? dumpNV21ToFile(frame, "preview_beauty_output",
            task.w, task.h, task.stride, task.scanline) : NO_ERROR;
        mPreviewPf ? PfLogger::getInstance()
            ->stop("preview_beauty_process") : NO_ERROR;
    } else if (task.type == FRAME_TYPE_SNAPSHOT) {
        mDumpSnapshot ? dumpNV21ToFile(frame, "snapshot_beauty_output",
            task.w, task.h, task.stride, task.scanline) : NO_ERROR;
        mSnapshotPf ? PfLogger::getInstance()
            ->stop("snapshot_beauty_process") : NO_ERROR;
    }

    if (SUCCEED(rc)) {
        result.valid = true;
    }

    return rc;
}

int32_t BeautyFace::setParm(ParmTypeT &parm)
{
    if (NOTNULL(mEngine) && NOTNULL(mEngineParm)) {
        if (!parm.camType.isNobody()) {
            mEngineParm->camType = parm.camType;
        }

        if (parm.manual >= 0) {
            mEngineParm->manualStrength = parm.manual;
        }
        if (parm.luxIndex >= 0) {
            mEngineParm->luxIndex = parm.luxIndex;
        }
        if (parm.backlight >= 0) {
            mEngineParm->isBacklight = !!parm.backlight;
        }
        if (parm.age >= 0) {
            mEngineParm->age = parm.age;
        }
        if (parm.gender != GENDER_TYPE_MAX_INVALID) {
            mEngineParm->gender = parm.gender;
        }

        if (parm.rotation < ROTATION_ANGLE_MAX_INVALID &&
            parm.rotation >= ROTATION_ANGLE_0) {
            mEngineParm->rotation = parm.rotation;
        }

        mEngine->setParm(*mEngineParm);
    } else {
        LOGE(mModule, "Engine not inited.");
    }

    return NO_ERROR;
}

int32_t BeautyFace::update(UpdateTypeT &/*update*/)
{
    // Don't care any updates from other algorithms
    return NO_ERROR;
}

int32_t BeautyFace::queryCaps(AlgCaps &caps)
{
    caps.frameNeeded    = SINGLE_FRAME_ALGORITHM_INPUT;
    caps.workingBufCnt  = 0;
    caps.workingBufSize = 0;
    caps.types   = FRAME_TYPE_MASK_PREVIEW | FRAME_TYPE_MASK_SNAPSHOT;
    caps.formats = FRAME_FORMAT_MASK_YUV_420_NV21 |
        FRAME_FORMAT_MASK_YUV_420_NV12 | FRAME_FORMAT_MASK_YUV_MONO;

    return NO_ERROR;
}

BeautyFace::GenderMask BeautyFace::getGenderMask(BeautyFace::Gender gender)
{
    return static_cast<GenderMask>(0x01 << gender);
}

BeautyFace::AutoManualMask BeautyFace::getAutoManualMask(BeautyFace::AutoManual mode)
{
    return static_cast<AutoManualMask>(0x01 << mode);
}

const char *BeautyFace::getGenderName(Gender gender)
{
    static const char *gGenderName[] = {
        [GENDER_TYPE_MALE]        = "male",
        [GENDER_TYPE_FEMALE]      = "female",
        [GENDER_TYPE_UNKNOWN]     = "unknown",
        [GENDER_TYPE_MAX_INVALID] = "max invalid",
    };

    if (gender < GENDER_TYPE_MALE || gender > GENDER_TYPE_MAX_INVALID) {
        LOGE(MODULE_ALGORITHM, "Invalid gender type %d", gender);
        gender = GENDER_TYPE_MAX_INVALID;
    }

    return gGenderName[gender];
}

int32_t BeautyFace::process(TaskTypeT &in, TaskTypeT &out, ResultTypeT &result)
{
    int32_t rc = process(in, result);
    if (SUCCEED(rc)) {
        if (ISNULL(out.data) || in.size > out.size) {
            LOGE(mModule, "Failed to copy result, "
                "NULL ptr or insufficient size, %p %d/%d",
                out.data, out.size, in.size);
            rc = PARAM_INVALID;
        }
    }

    if (SUCCEED(rc)) {
        void *data = out.data;
        out = in;
        memcpy(data, in.data, in.size);
        out.data = data;
    }

    return rc;
}

int32_t BeautyFace::queryStatus(StatusTypeT &/*status*/)
{
    // No status changed and not necessary to notify caller
    return NOT_SUPPORTED;
}

};

