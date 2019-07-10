#include "JpegSWEncoder.h"
#include "AlgorithmHelper.h"
#include "PfLogger.h"
#include "MemMgmt.h"
#include "turbojpeg.h"
#include "JpegExif.h"

namespace pandora {

#define AVERAGE_PERFORMANCE_COUNT 20
#define DEFAULT_JPEG_QUAL         100

static const uint8_t gExifHeader[] = { 0xff, 0xd8, 0xff, 0xe1 };

INIT_ALGORITHM_TRAITS_FUNC(JpegSWEncoder);

JpegSWEncoder::JpegSWEncoder(uint32_t argNum, va_list va) :
    mInited(false),
    mDump(false),
    mPf(false),
    mPfCnt(AVERAGE_PERFORMANCE_COUNT),
    mModule(MODULE_ALGORITHM),
    mEngine(NULL),
    mJpegBuf(NULL),
    mYuv420pBuf(NULL),
    mJpegBufSize(0),
    mPreBufSize(0),
    mPreYuvSize(0)
{
    for (uint32_t i = 0; i < argNum; i++) {
        LOGD(mModule, "va list arg %d value %d", i, va_arg(va, int32_t));
    }
}

JpegSWEncoder::~JpegSWEncoder()
{
    if (mInited) {
        deinit();
    }
}

int32_t JpegSWEncoder::init()
{
    int32_t rc = NO_ERROR;

    if (mInited) {
        rc = ALREADY_EXISTS;
    }

    if (SUCCEED(rc)) {
        rc = queryCaps(mCaps);
    }

    if (SUCCEED(rc)) {
        mInited = true;
    }

    return rc;
}

int32_t JpegSWEncoder::deinit()
{
    int32_t rc = NO_ERROR;

    if (!mInited) {
        rc = NOT_INITED;
    }

    if (SUCCEED(rc)) {
        if (NOTNULL(mJpegBuf)) {
            SECURE_FREE(mJpegBuf);
        }
    }

    if (NOTNULL(mYuv420pBuf)) {
        SECURE_FREE(mYuv420pBuf);
    }

    if (SUCCEED(rc)) {
        tjDestroy(mEngine);
    }

    if (SUCCEED(rc)) {
        mPreBufSize = 0;
        mJpegBufSize = 0;
        mPreYuvSize = 0;
        mInited = false;
    }

    return rc;
}

int32_t JpegSWEncoder::process(TaskTypeT & /*task*/, ResultTypeT & /*result*/)
{
    // Don't process on input buffer
    return NOT_SUPPORTED;
}

int32_t JpegSWEncoder::process(TaskTypeT &task, TaskTypeT &out, ResultTypeT &result)
{
    int32_t rc = NO_ERROR;
    uint32_t bufferSize = 0;
    uint32_t exifSize = 0;
    uint8_t *exifBuf = NULL;
    uint8_t *tmpBuf = NULL;
    IAlgorithmT<JpegExif> *algT = NULL;
    JpegExif *exif = NULL;

    if (!mInited) {
        rc = NOT_INITED;
    } else {
        result.valid = false;
        result.taskid = task.taskid;
    }

    if (SUCCEED(rc)) {
        if (ISNULL(mEngine)) {
            mEngine = tjInitCompress();
            if (ISNULL(mEngine)) {
                LOGE(mModule, "init compress engine failed");
                rc = NO_MEMORY;
            }
        }
    }

    if (SUCCEED(rc)) {
        bufferSize = tjBufSize(task.w, task.h, TJSAMP_420);
        if (bufferSize <= 0) {
            LOGE(mModule, "Failed to getBufferSize");
            rc = NOT_READY;
        }
    }

    if (SUCCEED(rc)) {
        if (bufferSize != mPreBufSize) {
            mJpegBufSize = bufferSize;
            mPreBufSize = bufferSize;

            if (NOTNULL(mJpegBuf)) {
                SECURE_FREE(mJpegBuf);
            }

            mJpegBuf = (unsigned char *)Malloc(mJpegBufSize);
            if (ISNULL(mJpegBuf)) {
                LOGE(mModule, "Failed to alloc jpeg buf %d", rc);
                rc = NO_MEMORY;
            }
        }
    }

    if (SUCCEED(rc)) {
        bufferSize = task.w * task.h * 1.5;

        if (mPreYuvSize != bufferSize) {
            if (NOTNULL(mYuv420pBuf)) {
                SECURE_FREE(mYuv420pBuf);
            }

            mYuv420pBuf = (uint8_t *)Malloc(bufferSize);
            if (ISNULL(mYuv420pBuf)) {
                LOGE(mModule, "Failed to alloc mYuv420pBuf buf %d", rc);
                rc = NO_MEMORY;
            } else {
                mPreYuvSize = bufferSize;
            }
        }
    }

    if (SUCCEED(rc)) {
        if (mDump) {
            dumpNV21ToFile((uint8_t *)task.data, "jpeg_sw_encoder-input",
                task.w, task.h, task.stride, task.scanline);
        }

        mPf ? PfLogger::getInstance()
            ->start("jpeg_sw_encoder_process", mPfCnt) : NO_ERROR;
    }

    if (SUCCEED(rc)) {
        yuv420spToYuv420p((uint8_t *)task.data, mYuv420pBuf, task.w, task.h);

        rc = tjCompressFromYUV(mEngine, mYuv420pBuf, task.w,
            task.stride, task.h, TJSAMP_420, &mJpegBuf,
            (unsigned long *)&mJpegBufSize, DEFAULT_JPEG_QUAL, TJFLAG_FASTUPSAMPLE);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Compress from yuv failed, rc is %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        mPf ? PfLogger::getInstance()
            ->stop("jpeg_sw_encoder_process") : NO_ERROR;

        if (mDump) {
            dumpJpegToFile(mJpegBuf, "jpeg_sw_encoder-output",
                task.w, task.h, mJpegBufSize);
        }
    }

    if (SUCCEED(rc)) {
        if (ISNULL(task.exif)) {
            memcpy(out.data, mJpegBuf, mJpegBufSize);
            out.w = task.w;
            out.h = task.h;
            out.size = mJpegBufSize;
            result.valid = true;
            LOGI(mModule, "encode jpeg without exif success");
            rc = JUMP_DONE;
        }
    }

    if (SUCCEED(rc)) {
        algT = static_cast<IAlgorithmT<JpegExif> *>(task.exif);
        if (ISNULL(algT)) {
            LOGE(mModule, "get Algorithm failed");
            rc = PARAM_INVALID;
        }
    }

    if (SUCCEED(rc)) {
        exif = algT->getAlgorithmInDanger();
        if (ISNULL(exif)) {
            LOGE(mModule, "Failed to get algorithm directly");
            rc = INTERNAL_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        rc = exif->waitAndReadExif(&exifBuf, &exifSize);
        if (FAILED(rc)) {
            LOGE(mModule, "get jpeg exif timeout");
            rc = TIMEDOUT;
        }
    }

    if (SUCCEED(rc)) {
        tmpBuf = (uint8_t *)out.data;
        tmpBuf[0] = gExifHeader[0];
        tmpBuf[1] = gExifHeader[1];
        tmpBuf[2] = gExifHeader[2];
        tmpBuf[3] = gExifHeader[3];
        tmpBuf[4] = (exifSize + sizeof(gExifHeader)) >> 8;
        tmpBuf[5] = (exifSize + sizeof(gExifHeader)) & 0xff;
        memcpy((uint8_t *)out.data + sizeof(gExifHeader) + 2 , exifBuf, exifSize);
    }

    if (SUCCEED(rc)) {
        memcpy((uint8_t *)out.data + sizeof(gExifHeader) + 2 + exifSize,
            mJpegBuf, mJpegBufSize);
        out.w = task.w;
        out.h = task.h;
        out.size = sizeof(gExifHeader) + 2 + exifSize + mJpegBufSize;
        result.valid = true;
        LOGI(mModule, "SW jpeg encode success");
    }

    if (mDump) {
        dumpJpegToFile((uint8_t *)out.data, "jpeg_sw_encoder_with_exif",
            task.w, task.h, out.size);
    }

    return RETURNIGNORE(rc, JUMP_DONE);
}

int32_t JpegSWEncoder::setParm(ParmTypeT &/*parm*/)
{
    // Not necessary to handle externally parms
    return NO_ERROR;
}

int32_t JpegSWEncoder::update(UpdateTypeT &/*update*/)
{
    // Don't care any updates from other algorithms
    return NO_ERROR;
}

int32_t JpegSWEncoder::queryCaps(AlgCaps &caps)
{
    caps.frameNeeded    = SINGLE_FRAME_ALGORITHM_INPUT;
    caps.workingBufCnt  = 0;
    caps.workingBufSize = 0;
    caps.types   = FRAME_TYPE_MASK_PREVIEW | FRAME_TYPE_MASK_SNAPSHOT;
    caps.formats = FRAME_FORMAT_MASK_YUV_420_NV21 | FRAME_FORMAT_MASK_YUV_420_NV12;

    return NO_ERROR;
}

int32_t JpegSWEncoder::queryStatus(StatusTypeT &/*status*/)
{
    // No status changed and not necessary to notify caller
    return NOT_SUPPORTED;
}

};


