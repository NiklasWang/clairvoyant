#include <stdarg.h>

#include "YUVRotator.h"
#include "AlgorithmHelper.h"
#include "PfLogger.h"

#include "libyuv.h"

namespace pandora {

#define AVERAGE_PERFORMANCE_COUNT 6

INIT_ALGORITHM_TRAITS_FUNC(YUVRotator);

template<typename K, typename V>
struct ParmMap {
    K key;
    V value;
};

static ParmMap<RotateOption, libyuv::RotationModeEnum> gConfigs[] = {
    {
        .key = YUV_ROTATOR_90,
        .value = libyuv::kRotate90,
    },
    {
        .key = YUV_ROTATOR_180,
        .value = libyuv::kRotate180,
    },
    {
        .key = YUV_ROTATOR_270,
        .value = libyuv::kRotate270,
    },
};

int32_t checkValidParm(ModuleType mModule, RotateOption option)
{
    int32_t rc = NO_ERROR;

    if (!(option == YUV_ROTATOR_90 ||
        option == YUV_ROTATOR_180 ||
        option == YUV_ROTATOR_270)) {
        LOGE(mModule, "Invalid option %d", option);
        rc = PARAM_INVALID;
    }

    return rc;
}

libyuv::RotationModeEnum mapConfig(RotateOption option)
{
    libyuv::RotationModeEnum result = libyuv::kRotate0;

    for (uint32_t i = 0; i < ARRAYSIZE(gConfigs); i++) {
        if (gConfigs[i].key == option) {
            result = gConfigs[i].value;
            break;
        }
    }

    return result;
}

YUVRotator::YUVRotator(uint32_t argNum, va_list va) :
    mInited(false),
    mModule(MODULE_ALGORITHM),
    mPf(false),
    mPfCnt(AVERAGE_PERFORMANCE_COUNT),
    mDump(false),
    mParmValid(false)
{
    if (argNum == 1) {
        mParms.option =
            static_cast<RotateOption>(va_arg(va, int32_t));
        ASSERT_LOG(mModule,
            SUCCEED(checkValidParm(mModule, mParms.option)),
            "INVALID size, suicide...");
        mParmValid = true;
    }
}

YUVRotator::~YUVRotator()
{
    if (mInited) {
        deinit();
    }
}

int32_t YUVRotator::init()
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

int32_t YUVRotator::deinit()
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

int32_t YUVRotator::process(TaskTypeT &task, ResultTypeT &result)
{
    int32_t rc = NO_ERROR;
    int32_t inputW = task.w;
    int32_t inputH = task.h;
    int32_t stride = task.stride;
    uint8_t *frame = (uint8_t *)task.data;
    uint8_t *buf = NULL;
    uint8_t *y1, *uv1;
    uint8_t *y2, *u2, *v2;
    int32_t outputW, outputH;

    if (!mInited) {
        rc = NOT_INITED;
    } else {
        result.valid = false;
    }

    if (SUCCEED(rc)) {
        if (!mParmValid) {
            LOGE(mModule, "Not received configurations,"
                " option %dx%d", mParms.option);
            rc = NOT_INITED;
        }
    }

    if (SUCCEED(rc)) {
        size_t size = getMaximumFrameLen(
            task.w, task.h, task.stride, task.scanline);
        rc = mBuf.replace(0, task,
            size >= task.size ? size : task.size);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to cache input frame, %d", rc);
        } else {
            buf = (uint8_t *)mBuf[0];
        }
    }

    if (SUCCEED(rc)) {
        mDump ? dumpNV21ToFile(buf, "rotator_input",
            inputW, inputH, stride, task.scanline) : NO_ERROR;
        mPf ? PfLogger::getInstance()
            ->start("rotator_process", mPfCnt) : NO_ERROR;
    }

    if (SUCCEED(rc)) {
        y1 = getNV21YData(frame,
            inputW, inputH, stride);
        uv1 = getNV21UVData(frame,
            inputW, inputH, stride);
    }

    if (SUCCEED(rc)) {
        if (mParms.option == YUV_ROTATOR_90 ||
            mParms.option == YUV_ROTATOR_270) {
            outputW = inputH;
            outputH = inputW;
        } else {
            outputW = inputW;
            outputH = inputH;
        }
    }

    if (SUCCEED(rc)) {
        y2 = getI420YData(buf,
            outputW, outputH, stride, stride, stride);
        u2 = getI420UData(buf,
            outputW, outputH, stride, stride, stride);
        v2 = getI420VData(buf,
            outputW, outputH, stride, stride, stride);
    }

    if (SUCCEED(rc)) {
        int32_t width1 = getAlignedStride(inputW, stride);
        int32_t width2 = getAlignedStride(outputW, stride);
        rc = libyuv::NV12ToI420Rotate(
            y1, width1, uv1, width1,
            y2, width2, u2, width2/2, v2, width2/2,
            inputW, inputH, mapConfig(mParms.option));
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to rotate, %dx%d, %d",
                inputW, inputH, rc);
        }
    }

    if (SUCCEED(rc)) {
        y1 = getI420YData(frame,
            outputW, outputH, stride, stride, stride);
        uv1 = getNV21UVData(frame,
            outputW, outputH, stride);
    }

    if (SUCCEED(rc)) {
        int32_t width1 = getAlignedStride(outputW, stride);
        int32_t width2 = getAlignedStride(outputW, stride);
        rc = libyuv::I420ToNV12(y2, width2, u2, width2/2, v2, width2/2,
               y1, width1, uv1, width1, outputW, outputH);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to convert I420 to NV12, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        mDump ? dumpNV21ToFile(frame,
            "rotator_output", outputW, outputH,
                stride, task.scanline) : NO_ERROR;
        mPf ? PfLogger::getInstance()
            ->stop("rotator_process") : NO_ERROR;
    }

    if (SUCCEED(rc)) {
        result.w = outputW;
        result.h = outputH;
        result.size = getFrameLen2(outputW, outputH, stride);
        result.data = task.data;
        result.valid = true;
    }

    return rc;
}

int32_t YUVRotator::setParm(ParmTypeT &parm)
{
    mParms = parm;

    if (!SUCCEED(checkValidParm(mModule, mParms.option))) {
        mParmValid = false;
    } else {
        mParmValid = true;
    }

    return NO_ERROR;
}

int32_t YUVRotator::update(UpdateTypeT &/*update*/)
{
    // Don't care any updates from other algorithms
    return NO_ERROR;
}

int32_t YUVRotator::queryCaps(AlgCaps &caps)
{
    caps.frameNeeded =  SINGLE_FRAME_ALGORITHM_INPUT;
    caps.workingBufCnt = 0;
    caps.workingBufSize = 0;
    caps.types   = FRAME_TYPE_MASK_PREVIEW | FRAME_TYPE_MASK_SNAPSHOT | FRAME_TYPE_MASK_VIDEO;
    caps.formats = FRAME_FORMAT_MASK_YUV_420_NV21 | FRAME_FORMAT_MASK_YUV_420_NV12;

    return NO_ERROR;
}

int32_t YUVRotator::process(TaskTypeT &in, TaskTypeT &out, ResultTypeT &result)
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

int32_t YUVRotator::queryStatus(StatusTypeT &/*status*/)
{
    // No status changed and not necessary to notify caller
    return NOT_SUPPORTED;
}

};

