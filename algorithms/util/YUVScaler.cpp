#include <math.h>
#include <stdarg.h>

#include "YUVScaler.h"
#include "AlgorithmHelper.h"
#include "PfLogger.h"

#include "libyuv.h"

namespace pandora {

#define AVERAGE_PERFORMANCE_COUNT 6

INIT_ALGORITHM_TRAITS_FUNC(YUVScaler);

int32_t checkValidParm(
    ModuleType mModule, int32_t outputW, int32_t outputH)
{
    int32_t rc = NO_ERROR;

    if (!(outputW > 0) || !(outputH > 0)) {
        LOGE(mModule, "Invalid size output %dx%d", outputW, outputH);
        rc = PARAM_INVALID;
    }

    return rc;
}

YUVScaler::YUVScaler(uint32_t argNum, va_list va) :
    mInited(false),
    mModule(MODULE_ALGORITHM),
    mPf(false),
    mPfCnt(AVERAGE_PERFORMANCE_COUNT),
    mDump(false),
    mParmValid(false)
{
    if (argNum == 2) {
        mParms.outputW = va_arg(va, int32_t);
        mParms.outputH = va_arg(va, int32_t);
        ASSERT_LOG(mModule,
            SUCCEED(checkValidParm(mModule,
                mParms.outputW, mParms.outputH)),
            "INVALID size, suicide...");
        mParmValid = true;
    }
}

YUVScaler::~YUVScaler()
{
    if (mInited) {
        deinit();
    }
}

int32_t YUVScaler::init()
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

int32_t YUVScaler::deinit()
{
    int32_t rc = NO_ERROR;

    if (!mInited) {
        rc = NOT_INITED;
    }

    if (SUCCEED(rc)) {
        mBuf.clear();
    }

    if (SUCCEED(rc)) {
        mInited = false;
    }

    return rc;
}

int32_t YUVScaler::process(TaskTypeT &task, ResultTypeT &result)
{
    int32_t rc = NO_ERROR;
    int32_t inputW = task.w;
    int32_t inputH = task.h;
    int32_t stride = task.stride;
    uint8_t *frame = (uint8_t *)task.data;
    uint8_t *buf = NULL;
    uint8_t *y1, *uv1, *u1, *v1;
    uint8_t *y2, *uv2, *u2, *v2;

    if (!mInited) {
        rc = NOT_INITED;
    } else {
        result.valid = false;
    }

    if (SUCCEED(rc)) {
        if (!mParmValid) {
            LOGE(mModule, "Not received configurations."
                "output %dx%d", mParms.outputW, mParms.outputH);
            rc = NOT_INITED;
        }
    }

    if (SUCCEED(rc)) {
        size_t size = getFrameLen(mParms.outputW,
            mParms.outputH, task.stride, task.scanline);
        if (size > task.size) {
            LOGE(mModule, "Memory insufficient, "
                "input frame %dx%d output frame %dx%d,"
                "%d bytes owned, %d bytes required.",
                task.w, task.h, mParms.outputW,
                mParms.outputH, task.size, size);
            rc = PARAM_INVALID;
        }
    }

    if (SUCCEED(rc)) {
        rc = mBuf.replace(0, task);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to cache input frame, %d", rc);
        } else {
            buf = (uint8_t *)mBuf[0];
        }
    }

    if (SUCCEED(rc)) {
        mDump ? dumpNV21ToFile(buf, "scaler_input",
            inputW, inputH, stride, task.scanline) : NO_ERROR;
        mPf ? PfLogger::getInstance()
            ->start("scaler_process", mPfCnt) : NO_ERROR;
    }

    if (SUCCEED(rc)) {
        y1 = getNV21YData(frame,
            inputW, inputH, stride);
        uv1 = getNV21UVData(frame,
            inputW, inputH, stride);
        u1 = getI420UData(frame, inputW,
            inputH, stride, stride, stride);
        v1 = getI420VData(frame, inputW,
            inputH, stride, stride, stride);
        y2 = getI420YData(buf, inputW,
            inputH, stride, stride, stride);
        uv2 = getNV21UVData(buf,
            inputW, inputH, stride);
        u2 = getI420UData(buf, inputW,
            inputH, stride, stride, stride);
        v2 = getI420VData(buf, inputW,
            inputH, stride, stride, stride);
    }

    if (SUCCEED(rc)) {
        int32_t ywidth = getAlignedStride(inputW, stride);
        rc = libyuv::NV12ToI420(y2, ywidth, uv2, ywidth,
            y1, ywidth, u1, ywidth/2, v1, ywidth/2, inputW, inputH);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to convert NV12 to I420, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        y2 = getI420YData(buf, mParms.outputW,
            mParms.outputH, stride, stride, stride);
        u2 = getI420UData(buf, mParms.outputW,
            mParms.outputH, stride, stride, stride);
        v2 = getI420VData(buf, mParms.outputW,
            mParms.outputH, stride, stride, stride);
    }

    if (SUCCEED(rc)) {
        int32_t width1 = getAlignedStride(inputW, stride);
        int32_t width2 = getAlignedStride(mParms.outputW, stride);
        rc = libyuv::I420Scale(
            y1, width1, u1, width1/2, v1, width1/2, inputW, inputH,
            y2, width2, u2, width2/2, v2, width2/2,
            mParms.outputW, mParms.outputH, libyuv::kFilterBilinear);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to scale, %dx%d -> %dx%d, %d",
                inputW, inputH, mParms.outputW, mParms.outputH, rc);
        }
    }

    if (SUCCEED(rc)) {
        y1 = getNV21YData(frame,
            mParms.outputW, mParms.outputH, stride);
        uv1 = getNV21UVData(frame,
            mParms.outputW, mParms.outputH, stride);
    }

    if (SUCCEED(rc)) {
        int32_t width = getAlignedStride(mParms.outputW, stride);
        rc = libyuv::I420ToNV12(y2, width, u2, width/2, v2, width/2,
               y1, width, uv1, width,
               mParms.outputW, mParms.outputH);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to convert I420 to NV12, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        mDump ? dumpNV21ToFile(frame,
            "scaler_output", mParms.outputW, mParms.outputH,
                stride, task.scanline) : NO_ERROR;
        mPf ? PfLogger::getInstance()
            ->stop("scaler_process") : NO_ERROR;
    }

    if (SUCCEED(rc)) {
        result.w = mParms.outputW;
        result.h = mParms.outputH;
        result.size = getFrameLen2(result.w, result.h, stride);
        result.data = frame;
        result.valid = true;
    }

    return rc;
}

int32_t YUVScaler::setParm(ParmTypeT &parm)
{
    mParms = parm;

    if (!SUCCEED(checkValidParm(mModule,
            mParms.outputW, mParms.outputH))) {
        mParmValid = false;
    } else {
        mParmValid = true;
    }

    return NO_ERROR;
}

int32_t YUVScaler::update(UpdateTypeT &/*update*/)
{
    // Don't care any updates from other algorithms
    return NO_ERROR;
}

int32_t YUVScaler::queryCaps(AlgCaps &caps)
{
    caps.frameNeeded =  SINGLE_FRAME_ALGORITHM_INPUT;
    caps.workingBufCnt = 0;
    caps.workingBufSize = 0;
    caps.types   = FRAME_TYPE_MASK_PREVIEW | FRAME_TYPE_MASK_SNAPSHOT | FRAME_TYPE_MASK_VIDEO;
    caps.formats = FRAME_FORMAT_MASK_YUV_420_NV21 | FRAME_FORMAT_MASK_YUV_420_NV12;

    return NO_ERROR;
}

int32_t YUVScaler::process(TaskTypeT &in, TaskTypeT &out, ResultTypeT &result)
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

int32_t YUVScaler::queryStatus(StatusTypeT &/*status*/)
{
    // No status changed and not necessary to notify caller
    return NOT_SUPPORTED;
}

};

