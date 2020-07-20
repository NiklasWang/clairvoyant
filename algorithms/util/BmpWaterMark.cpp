#include <stdlib.h>
#include "BmpWaterMark.h"
#include "AlgorithmHelper.h"
#include "PfLogger.h"
#include "Logo.h"

#include "Bmp2Yuv.h"
#include "YUVScaler.h"
#include "YUVRotator.h"
#include "Algorithm.h"

namespace pandora {

INIT_ALGORITHM_TRAITS_FUNC(BmpWaterMark);

#define AVERAGE_PERFORMANCE_COUNT 10

#define Y_BLACK       (0x10)
#define UV_DESERVED   (0x80)

BmpWaterMarkPosition gBmpPositionMaps[][4] = {
    // 0                                   90                                    180                                   270
    {BMP_WATER_MARK_POSITION_TOP_LEFT,     BMP_WATER_MARK_POSITION_BOTTOM_LEFT,  BMP_WATER_MARK_POSITION_BOTTOM_RIGHT, BMP_WATER_MARK_POSITION_TOP_RIGHT    }, // position top left
    {BMP_WATER_MARK_POSITION_BOTTOM_LEFT,  BMP_WATER_MARK_POSITION_BOTTOM_RIGHT, BMP_WATER_MARK_POSITION_TOP_RIGHT,    BMP_WATER_MARK_POSITION_TOP_LEFT     }, // position bottom left
    {BMP_WATER_MARK_POSITION_BOTTOM_RIGHT, BMP_WATER_MARK_POSITION_TOP_RIGHT,    BMP_WATER_MARK_POSITION_TOP_LEFT,     BMP_WATER_MARK_POSITION_BOTTOM_LEFT  }, // position bottom right
    {BMP_WATER_MARK_POSITION_TOP_RIGHT,    BMP_WATER_MARK_POSITION_TOP_LEFT,     BMP_WATER_MARK_POSITION_BOTTOM_LEFT,  BMP_WATER_MARK_POSITION_BOTTOM_RIGHT }, // position top right
    {BMP_WATER_MARK_POSITION_MIDDLE,       BMP_WATER_MARK_POSITION_MIDDLE,       BMP_WATER_MARK_POSITION_MIDDLE,       BMP_WATER_MARK_POSITION_MIDDLE       }, // position middle
};

BmpWaterMarkPosition gBMirrorPositionMaps[5] = {
    BMP_WATER_MARK_POSITION_TOP_RIGHT,      // position top left
    BMP_WATER_MARK_POSITION_BOTTOM_RIGHT,   // position bottom left
    BMP_WATER_MARK_POSITION_BOTTOM_LEFT,    // position bottom right
    BMP_WATER_MARK_POSITION_TOP_LEFT,       // position top right
    BMP_WATER_MARK_POSITION_MIDDLE,         // position middle
};

BmpWaterMark::BmpWaterMark(uint32_t argNum, va_list va) :
    mInited(false),
    mModule(MODULE_ALGORITHM),
    mPf(false),
    mPfCnt(AVERAGE_PERFORMANCE_COUNT),
    mDump(false),
    mSize(5),
    mUnscaledYuv(NULL),
    mUnscaledMirrorYuv(NULL),
    mUnscaledFrame(NULL),
    mScaler(NULL),
    mRotator(NULL),
    mScaledYuv(NULL),
    mScaledFrame(NULL),
    mRotatedYuv(NULL),
    mRotatedFrame(NULL),
    mParmValid(false),
    mRotation(ROTATION_ANGLE_0),
    mPosition(BMP_WATER_MARK_POSITION_TOP_RIGHT),
    mLastRotation(ROTATION_ANGLE_0)
{
    if (argNum == 4) {
        mBmp = (uint8_t *)va_arg(va, uint8_t*);
        ASSERT_LOG(mModule, NOTNULL(mBmp), "Bmp can't be NULL, suicide...");

        int32_t position = va_arg(va, int32_t);
        if (position >= 0 &&
            position < BMP_WATER_MARK_POSITION_MAX_INVALID) {
            mPosition = static_cast<BmpWaterMarkPosition>(position);
        } else {
            LOGE(mModule, "Invalid position %d, reset to top right",
                position);
        }

        int32_t rotation = va_arg(va, int32_t);
        if (rotation >= ROTATION_ANGLE_0 &&
            rotation < ROTATION_ANGLE_MAX_INVALID ) {
            mRotation = static_cast<RotationAngle>(rotation);
        } else {
            LOGE(mModule, "Invalid rotation %d, reset to angle 0",
                position);
        }

        int32_t size = va_arg(va, int32_t);
        if (size > 0 && size <= 100) {
            mSize = size;
        }
    }
}

BmpWaterMark::~BmpWaterMark()
{
    if (mInited) {
        deinit();
    }
}

int32_t BmpWaterMark::init()
{
    int32_t rc = NO_ERROR;
    uint32_t bmpW = 0, bmpH = 0;
    uint8_t *yuvBuf = NULL;
    size_t yuvBufSize = 0;

    if (mInited) {
        rc = ALREADY_EXISTS;
    }

    if (SUCCEED(rc)) {
        rc = getBmpWidthHeight(mBmp, &bmpW, &bmpH);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to get bmp WxH");
        }
    }

    if (SUCCEED(rc)) {
        yuvBufSize = getFrameLen2(bmpW, bmpH, 2);
        yuvBuf = (uint8_t *)Malloc(yuvBufSize);
        if (ISNULL(yuvBuf)) {
            LOGE(mModule, "Failed to alloc %d bytes for full "
                "size yuv.", yuvBufSize);
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        rc = Bmp2Yuv(mBmp, yuvBuf);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to convert bmp 2 yuv, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        mUnscaledYuv = yuvBuf;
        mUnscaledFrame = new FrameInfo(
            mUnscaledYuv, bmpW, bmpH, 2, 2, yuvBufSize,
            FRAME_FORMAT_YUV_420_NV21, FRAME_TYPE_SNAPSHOT, 0LL);
        if (ISNULL(mUnscaledFrame)) {
            LOGE(mModule, "Failed to alloc %d bytes for full yuv inf.",
                sizeof(FrameInfo));
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        if (mDump) {
            dumpNV21ToFile(mUnscaledYuv,
                "unscaled_water_mark", bmpW, bmpH, 2, 2);
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

int32_t BmpWaterMark::deinit()
{
    int32_t rc = NO_ERROR;

    if (!mInited) {
        rc = NOT_INITED;
    }

    if (SUCCEED(rc)) {
        SECURE_FREE(mUnscaledYuv);
        SECURE_FREE(mUnscaledMirrorYuv);
        SECURE_DELETE(mUnscaledFrame);
        SECURE_FREE(mScaledYuv);
        SECURE_DELETE(mScaledFrame);
        SECURE_FREE(mRotatedYuv);
        SECURE_DELETE(mRotatedFrame);
        // Scaler will removed by sp<>
    }

    if (SUCCEED(rc)) {
        mInited = false;
    }

    return rc;
}

int32_t calcWaterMarkSize(int32_t inputW, int32_t inputH, int32_t origW, int32_t origH,
        Ratio ratio, int32_t rotation, int32_t *outputW, int32_t *outputH)
{
    if (rotation == 1 || rotation == 3) {
        *outputH = ratio.numerator * inputH / ratio.denominator;
        if (abs(*outputH - origH) <= 2) {
            *outputH = origH;
        }
        *outputH = getAlignedStride(*outputH, 2);
        *outputW = *outputH * origW / origH;
        *outputW = getAlignedStride(*outputW, 2);
    } else {
        *outputW = ratio.numerator * inputW / ratio.denominator;
        if (abs(*outputW - origW) <= 2) {
            *outputW = origW;
        }
        *outputW = getAlignedStride(*outputW, 2);
        *outputH = *outputW * origH / origW;
        *outputH = getAlignedStride(*outputH, 2);
    }

    return NO_ERROR;
}

int32_t calcStartXY(int32_t w, int32_t h, int32_t wmW, int32_t wmH, int32_t dp, int32_t dpi,
    int32_t *startX, int32_t *startY, BmpWaterMarkPosition position)
{
    int32_t x, y;
    int32_t margin = (dpi / 160) * dp;

    switch (position) {
        case BMP_WATER_MARK_POSITION_MIDDLE: {
            x = w / 2 - wmW / 2;
            y = h / 2 - wmH / 2;
        } break;
        case BMP_WATER_MARK_POSITION_TOP_LEFT: {
            x = 0;
            y = 0;
            x += margin;
            y += margin;
        } break;
        case BMP_WATER_MARK_POSITION_TOP_RIGHT: {
            x = w - wmW;
            y = 0;
            x -= margin;
            y += margin;
        } break;
        case BMP_WATER_MARK_POSITION_BOTTOM_LEFT: {
            x = 0;
            y = h - wmH;
            x += margin;
            y -= margin;
        } break;
        case BMP_WATER_MARK_POSITION_BOTTOM_RIGHT:
        default: {
            x = w - wmW;
            y = h - wmH;
            x -= margin;
            y -= margin;
        }
    }

    if (x + wmW > w) {
        x = w - wmW;
    }

    if (y + wmH > h) {
        y = h - wmH;
    }

    x = x > 0 ? x : 0;
    y = y > 0 ? y : 0;

    *startX = x;
    *startY = y;

    return NO_ERROR;
}

int32_t BmpWaterMark::process(TaskTypeT &task, ResultTypeT &result)
{
    int32_t rc = NO_ERROR;
    uint8_t *frame = (uint8_t *)task.data;
    int32_t wmW, wmH;
    size_t wmlen;
    bool needScale = true;
    bool scaled = true;
    uint8_t *scaleBuf = NULL;
    int32_t startX, startY;
    BmpWaterMarkPosition position;

    if (!mInited) {
        rc = NOT_INITED;
    } else {
        result.valid = false;
        result.taskid = task.taskid;
    }

     if (SUCCEED(rc)) {
        if (!mParmValid) {
            LOGE(mModule, "Not received configurations.");
            rc = NOT_INITED;
        }
    }

    if (SUCCEED(rc)) {
        if (mParm.needMirror && ISNULL(mUnscaledMirrorYuv)) {
            mUnscaledMirrorYuv = (uint8_t *)Malloc(mUnscaledFrame->size);
            if (ISNULL(mUnscaledMirrorYuv)) {
                LOGE(mModule, "Failed to alloc %d bytes for mirror yuv.",
                    mUnscaledFrame->size);
                rc = NO_MEMORY;
            }
        }
    }

    if (SUCCEED(rc)) {
        if (mParm.needMirror && NOTNULL(mUnscaledMirrorYuv)) {
            int32_t wmW = mUnscaledFrame->w;
            int32_t wmH = mUnscaledFrame->h;
            uint8_t *y1 = getNV21YData(mUnscaledYuv, wmW, wmH, 2, 2);
            uint8_t *uv1 = getNV21UVData(mUnscaledYuv, wmW, wmH, 2, 2);

            uint8_t *y2 = getNV21YData(mUnscaledMirrorYuv, wmW, wmH, 2, 2);
            uint8_t *uv2 = getNV21UVData(mUnscaledMirrorYuv, wmW, wmH, 2, 2);

            for (int32_t j = 0; j < wmH; j++) {
                for (int32_t i = 0; i < wmW; i++) {
                    y2[j * wmW + i] = y1[j * wmW + wmW - i - 1];
                    if ((j % 2) == 0 && (i % 2) == 0) {
                        uv2[(j / 2) * wmW + i] = uv1[(j / 2) * wmW +  wmW - i - 2];
                        uv2[(j / 2) * wmW + i + 1] = uv1[(j / 2) * wmW +  wmW - i - 1];
                    }
                }
            }
        }
    }

    if (SUCCEED(rc)) {
        if (ISNULL(mRotatedYuv)) {
            mRotatedYuv = (uint8_t *)Malloc(mUnscaledFrame->size);
            if (ISNULL(mRotatedYuv)) {
                LOGE(mModule, "Failed to alloc %d bytes for rotated yuv.",
                    mUnscaledFrame->size);
                rc = NO_MEMORY;
            }
        }
    }

    if (SUCCEED(rc)) {
        if (ISNULL(mRotatedFrame)) {
            mRotatedFrame = new FrameInfo(mRotatedYuv,
                mUnscaledFrame->w, mUnscaledFrame->h, 2, 2,
                mUnscaledFrame->size, FRAME_FORMAT_YUV_420_NV21,
                FRAME_TYPE_SNAPSHOT, 0LL);
            if (ISNULL(mRotatedFrame)) {
                LOGE(mModule, "Failed to alloc %d bytes for rotated yuv inf.",
                    sizeof(FrameInfo));
                rc = NO_MEMORY;
            }
        } else {
            mRotatedFrame->w = mUnscaledFrame->w;
            mRotatedFrame->h = mUnscaledFrame->h;
        }
    }

    if (SUCCEED(rc)) {
        if (NOTNULL(mRotatedYuv) && NOTNULL(mRotatedFrame)) {
            if (!mParm.needMirror)
                memcpy(mRotatedYuv, mUnscaledYuv, mUnscaledFrame->size);
            else
                memcpy(mRotatedYuv, mUnscaledMirrorYuv, mUnscaledFrame->size);
        }
    }

    if (SUCCEED(rc)) {
        if (mPosition >= BMP_WATER_MARK_POSITION_MAX_INVALID ||
            mRotation < ROTATION_ANGLE_0 || mRotation > ROTATION_ANGLE_270) {
            LOGE(mModule, "rotation angle %d or position %d is not right",
                mRotation, mPosition);
            rc = PARAM_INVALID;
        } else {
            position = mPosition;
            if (mParm.needMirror) {
                position = gBMirrorPositionMaps[position];
            }
            position = gBmpPositionMaps[position][mRotation];
        }
    }

    if (SUCCEED(rc) && mRotation) {
        AlgTraits<YUVRotator>::ParmType parm;
        switch (mRotation) {
            case ROTATION_ANGLE_90:
                parm.option = YUV_ROTATOR_270;
                break;
            case ROTATION_ANGLE_180:
                parm.option = YUV_ROTATOR_180;
                break;
            case ROTATION_ANGLE_270:
                parm.option = YUV_ROTATOR_90;
                break;
            default:
                LOGE(mModule, "rotation angle is not right: %d", mRotation);
                rc = INTERNAL_ERROR;
        }

        if (SUCCEED(rc)) {
            if (ISNULL(mRotator)) {
                mRotator = Algorithm<YUVRotator>::create(
                    "YUVRotator", "YUVRotatorThread", false, 1,
                    1, YUV_ROTATOR_90);
                if (ISNULL(mRotator)) {
                    LOGE(mModule, "Failed to create rotator.");
                    rc = INTERNAL_ERROR;
                }
            }
        }

        if (SUCCEED(rc)) {
            if (NOTNULL(mRotator)) {
                rc = mRotator->config(parm);
                if (!SUCCEED(rc)) {
                    LOGE(mModule, "Failed to config %s, %d",
                        mRotator->whoamI(), rc);
                }
            }
        }

        if (SUCCEED(rc)) {
            GlobalTaskType t(mRotatedYuv,
                mRotatedFrame->w, mRotatedFrame->h,
                mRotatedFrame->stride, mRotatedFrame->scanline,
                getFrameLen2(mRotatedFrame->w, mRotatedFrame->h,
                    mRotatedFrame->stride, mRotatedFrame->scanline),
                mRotatedFrame->format, mRotatedFrame->type);
            rc = mRotator->process(t);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to rotate frame %d, %d", mRotation, rc);
                rc = INTERNAL_ERROR;
            } else {
                if (mRotation == ROTATION_ANGLE_90 ||
                    mRotation == ROTATION_ANGLE_270) {
                    int32_t tmp = mRotatedFrame->w;
                    mRotatedFrame->w = mRotatedFrame->h;
                    mRotatedFrame->h = tmp;
                }
            }
        }
    }

    if (SUCCEED(rc)) {
        if (mDump) {
            dumpNV21ToFile(mRotatedYuv, "Rotate_water_mark",
                mRotatedFrame->w, mRotatedFrame->h,
                mRotatedFrame->stride, mRotatedFrame->scanline);
        }
    }

    if (SUCCEED(rc)) {
        rc = calcWaterMarkSize(task.w, task.h,
            mRotatedFrame->w, mRotatedFrame->h,
            mParm.wmRatio, mRotation, &wmW, &wmH);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to get water mark size, %d", rc);
        } else {
            if (wmW == mRotatedFrame->w && wmH == mRotatedFrame->h) {
                needScale = false;
            }
        }
    }

    if (SUCCEED(rc)) {
        wmlen = getFrameLen2(wmW, wmH, 2);
        if (ISNULL(mScaledFrame) ||
            wmW != mScaledFrame->w ||
            wmH != mScaledFrame->h ||
            wmlen != mScaledFrame->size ||
            mLastRotation != mRotation) {
            scaled = false;
            mLastRotation = mRotation;
            SECURE_FREE(mScaledYuv);
            SECURE_DELETE(mScaledFrame);
        }
    }

    if (SUCCEED(rc)) {
        if (ISNULL(mScaledYuv)) {
            mScaledYuv = (uint8_t *)Malloc(wmlen);
            if (ISNULL(mScaledYuv)) {
                LOGE(mModule, "Failed to alloc %d bytes for scaled yuv.",
                    wmlen);
                rc = NO_MEMORY;
            }
        }
    }

    if (SUCCEED(rc)) {
        if (ISNULL(mScaledFrame)) {
            mScaledFrame = new FrameInfo(mScaledYuv, wmW, wmH, 2, 2, wmlen,
                FRAME_FORMAT_YUV_420_NV21, FRAME_TYPE_SNAPSHOT, 0LL);
            if (ISNULL(mScaledFrame)) {
                LOGE(mModule, "Failed to alloc %d bytes for scaled yuv inf.",
                    sizeof(FrameInfo));
                rc = NO_MEMORY;
            }
        }
    }

    if (SUCCEED(rc)) {
        if (needScale && !scaled) {
            scaleBuf = (uint8_t *)Malloc(mRotatedFrame->size);
            if (ISNULL(scaleBuf)) {
                LOGE(mModule, "Failed to alloc %d bytes for yuv scaler.",
                    mRotatedFrame->size);
                rc = NO_MEMORY;
            } else {
                memcpy(scaleBuf, mRotatedYuv, mRotatedFrame->size);
            }
        }
    }

    if (SUCCEED(rc)) {
        if (needScale && !scaled && ISNULL(mScaler)) {
            mScaler = Algorithm<YUVScaler>::create(
                "YUVScaler", "YUVScalerThread",
                false, 1, 2, wmW, wmH);
            if (ISNULL(mScaler)) {
                LOGE(mModule, "Failed to create scaler.");
                rc = INTERNAL_ERROR;
            }
        }
    }

    if (SUCCEED(rc)) {
        if (needScale && !scaled && NOTNULL(mScaler)) {
            AlgTraits<YUVScaler>::ParmType parm;
            parm.outputW = wmW;
            parm.outputH = wmH;
            rc = mScaler->config(parm);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to config %s, %d",
                    mScaler->whoamI(), rc);
            }
        }
    }

    if (SUCCEED(rc)) {
        if (needScale && !scaled && NOTNULL(mScaler)) {
            GlobalTaskType t(scaleBuf, mRotatedFrame->w,
                mRotatedFrame->h, 2, 2, mRotatedFrame->size,
                mRotatedFrame->format, mRotatedFrame->type);
            rc = mScaler->process(t);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to scale image %dx%d to %dx%d.",
                        mUnscaledFrame->w, mUnscaledFrame->h, wmW, wmH);
                rc = INTERNAL_ERROR;
            } else {
                memcpy(mScaledYuv, scaleBuf, mScaledFrame->size);
            }
        }
    }

    if (SUCCEED(rc)) {
        if (!needScale) {
            memcpy(mScaledYuv, mRotatedYuv, mRotatedFrame->size);
        }
        if (mDump) {
            dumpNV21ToFile(mScaledYuv, "Scaled_water_mark",
                mScaledFrame->w, mScaledFrame->h,
                mScaledFrame->stride, mScaledFrame->scanline);
        }
    }

    if (SUCCEED(rc)) {
        rc = calcStartXY(task.w, task.h, mScaledFrame->w,
            mScaledFrame->h, mParm.dp, mParm.dpi,
            &startX, &startY, position);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to calc start X Y, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        mDump ? dumpNV21ToFile(frame, "bmp_water_mark_input",
            task.w, task.h, task.stride, task.scanline) : NO_ERROR;
        mPf ? PfLogger::getInstance()
            ->start("bmp_water_mark", mPfCnt) : NO_ERROR;
    }

    if (SUCCEED(rc)) {
        uint8_t *y1 = getNV21YData((uint8_t *)task.data,
            task.w, task.h, task.stride, task.scanline);
        uint8_t *uv1 = NULL;
        uint8_t *uv2 = NULL;
        if (task.format != FRAME_FORMAT_YUV_MONO) {
            uv1 = getNV21UVData((uint8_t *)task.data,
                task.w, task.h, task.stride, task.scanline);
            uv2 = getNV21UVData(mScaledYuv,
                mScaledFrame->w, mScaledFrame->h, 2, 2);
        }
        uint32_t ylen1 = getAlignedStride(task.w, task.stride);
        uint32_t uvlen1 = getAlignedStride(task.w, task.stride);
        uint8_t *y2 = getNV21YData(mScaledYuv,
            mScaledFrame->w, mScaledFrame->h, 2, 2);
        if (mParm.workmode == WM_WORK_COVERAGE) {
            for (int32_t i = 0; i < wmW; i++) {
                for (int32_t j = 0; j < wmH; j++) {
                    if (i + startX < task.w && j + startY < task.h && (y2[j * wmW + i] > Y_BLACK)) {
                        y1[(j + startY) * ylen1 + i + startX] = y2[j * wmW + i];
                        if ((j % 2) == 0 && task.format != FRAME_FORMAT_YUV_MONO) {
                            uv1[(j + startY) / 2 * uvlen1 + i + startX] = uv2[(j / 2) * wmW + i ];
                        }
                    }
                }
            }
        } else if (mParm.workmode == WM_WORK_TRANSPARENT) {
            for (int32_t i = 0; i < wmW; i++) {
                for (int32_t j = 0; j < wmH; j++) {
                    if (i + startX < task.w && j + startY < task.h && (y2[j * wmW + i] > Y_BLACK)) {
                        uint8_t wmY = y2[j * wmW + i];
                        uint8_t fY1 = (wmY * 1.0f / 255) * wmY;
                        uint8_t fY2 = ((255 - wmY) * 1.0f / 255) * y1[(j + startY) * ylen1 + i + startX];
                        y1[(j + startY) * ylen1 + i + startX] = (fY1 + fY2);
                    }
                }
            }
        } else if (mParm.workmode == WM_WORK_PUREWHITE) {
            for (int32_t i = 0; i < wmW; i++) {
                for (int32_t j = 0; j < wmH; j++) {
                    if (i + startX < task.w && j + startY < task.h && (y2[j * wmW + i] > Y_BLACK)) {
                        uint8_t wmY = y2[j * wmW + i];
                        uint8_t fY1 = (wmY * 1.0f / 255) * wmY;
                        uint8_t fY2 = ((255 - wmY) * 1.0f / 255) * y1[(j + startY) * ylen1 + i + startX];
                        y1[(j + startY) * ylen1 + i + startX] = (fY1 + fY2);
                        if ((j % 2) == 0 && task.format != FRAME_FORMAT_YUV_MONO) {
                            uv1[(j + startY) / 2 * uvlen1 + i + startX] = UV_DESERVED;
                        }
                    }
                }
            }
        }
    }

    if (SUCCEED(rc)) {
        mDump ? dumpNV21ToFile(frame, "bmp_water_mark_output",
            task.w, task.h, task.stride, task.scanline) : NO_ERROR;
        mPf ? PfLogger::getInstance()
            ->stop("bmp_water_mark") : NO_ERROR;
    }

    if (SUCCEED(rc) || !SUCCEED(rc)) {
        SECURE_FREE(scaleBuf);
    }

    if (SUCCEED(rc)) {
        result.valid = true;
    }

    return rc;
}

int32_t BmpWaterMark::setParm(ParmTypeT &parm)
{
    if (parm.rotation >= ROTATION_ANGLE_0 &&
        parm.rotation < ROTATION_ANGLE_MAX_INVALID) {
        mRotation = parm.rotation;
    }

    if (parm.position != BMP_WATER_MARK_POSITION_MAX_INVALID) {
        mPosition = parm.position;
    }

    mParm = parm;
    mParmValid = true;

    return NO_ERROR;
}

int32_t BmpWaterMark::update(UpdateTypeT &/*update*/)
{
    // Don't care any updates from other algorithms
    return NO_ERROR;
}

int32_t BmpWaterMark::queryCaps(AlgCaps &caps)
{
    caps.frameNeeded =  SINGLE_FRAME_ALGORITHM_INPUT;
    caps.workingBufCnt = 0;
    caps.workingBufSize = 0;
    caps.types   = FRAME_TYPE_MASK_PREVIEW |
        FRAME_TYPE_MASK_SNAPSHOT | FRAME_TYPE_MASK_VIDEO;
    caps.formats = FRAME_FORMAT_MASK_YUV_420_NV21 |
        FRAME_FORMAT_MASK_YUV_420_NV12 | FRAME_FORMAT_MASK_YUV_MONO;

    return NO_ERROR;
}

int32_t BmpWaterMark::process(TaskTypeT &in, TaskTypeT &out, ResultTypeT &result)
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

int32_t BmpWaterMark::queryStatus(StatusTypeT &/*status*/)
{
    // No status changed and not necessary to notify caller
    return NOT_SUPPORTED;
}

};

