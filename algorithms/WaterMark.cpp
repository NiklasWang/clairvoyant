#include "WaterMark.h"
#include "Watermark.h"
#include "Logo.h"
#include "AlgorithmHelper.h"
#include "PfLogger.h"

#include "BmpWaterMark.h"
#include "FontWaterMark.h"

#include "Algorithm.h"

namespace pandora {

#define AVERAGE_PERFORMANCE_COUNT 10
#define DEBUG_TEXT_MAX_LEN        33

INIT_ALGORITHM_TRAITS_FUNC(WaterMark);

WaterMark::WaterMark(uint32_t /*argNum*/, va_list /*va*/) :
    mInited(false),
    mModule(MODULE_ALGORITHM),
    mPf(false),
    mPfCnt(AVERAGE_PERFORMANCE_COUNT),
    mDump(false),
    mParmValid(false)
{
}

WaterMark::~WaterMark()
{
    if (mInited) {
        deinit();
    }
}

int32_t WaterMark::init()
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

int32_t WaterMark::deinit()
{
    int32_t rc = NO_ERROR;

    if (!mInited) {
        rc = NOT_INITED;
    }

    if (SUCCEED(rc)) {
        mInited = false;
    }

    return rc;
}

int32_t WaterMark::process(TaskTypeT &task, ResultTypeT &result)
{
    int32_t rc = NO_ERROR;
    uint8_t *frame = NULL;
    uint8_t *bmp   = NULL;
    bool needMirror = false;

    GlobalTaskType t(task.data,
        task.w, task.h, task.stride, task.scanline,
        task.size, task.format, task.type);

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
        if (mParm.info.camType.isFrontCamera() &&
            task.type == FRAME_TYPE_PREVIEW) {
            needMirror = true;
        }
    }

    if (SUCCEED(rc)) {
        mDump ? dumpNV21ToFile(task.data, "water_mark_input",
            task.w, task.h, task.stride, task.scanline) : NO_ERROR;
        mPf ? PfLogger::getInstance()
            ->start("water_mark_process", mPfCnt) : NO_ERROR;
    }

    if (SUCCEED(rc)) {
        if (mParm.info.enableProduct && ISNULL(mWaterMark)) {
            bmp = NULL;
            uint64_t bmpSize = 0;
            getWaterMark(mParm.info.id, &bmp, &bmpSize);
            if (ISNULL(bmp) || !bmpSize) {
                LOGE(mModule, "Failed to get water mark");
                rc = INTERNAL_ERROR;
            }
        }
    }

    if (SUCCEED(rc)) {
        if (mParm.info.enableProduct && ISNULL(mWaterMark)) {
            mWaterMark = Algorithm<BmpWaterMark>::create(
                "BmpWaterMark", "BmpWaterMarkThread", false, 1, 4,
                bmp,
                BMP_WATER_MARK_POSITION_BOTTOM_LEFT,
                ROTATION_ANGLE_0,
                20);
            if (ISNULL(mWaterMark)) {
                LOGE(mModule, "Failed to create bmp water mark algorithm");
                rc = INTERNAL_ERROR;
            }
        }
    }

    if (SUCCEED(rc)) {
        if (mParm.info.enableProduct && NOTNULL(mWaterMark)) {
            AlgTraits<BmpWaterMark>::ParmType parm;
            parm.rotation   = mParm.rotation;
            parm.dp         = mParm.info.dp;
            parm.dpi        = mParm.info.dpi;
            parm.needMirror = needMirror;
            parm.wmRatio    = mParm.info.wmRatio;
            parm.position   = BMP_WATER_MARK_POSITION_BOTTOM_LEFT;
            parm.workmode   = WM_WORK_PUREWHITE;
            rc = mWaterMark->config(parm);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to config %s, %d",
                    mWaterMark->whoamI(), rc);
            }
        }
    }

    if (SUCCEED(rc)) {
        if (mParm.info.enableProduct && NOTNULL(mWaterMark)) {
            rc = mWaterMark->process(t);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to draw logo, %d", rc);
                rc = INTERNAL_ERROR;
            }
        }
    }
    if (SUCCEED(rc)) {
        if (mParm.info.enableLogo && ISNULL(mLogo)) {
            bmp = NULL;
            uint64_t bmpSize = 0;
            getPandoraLogo(&bmp, &bmpSize);
            if (ISNULL(bmp) || !bmpSize) {
                LOGE(mModule, "Failed to get pandora log");
                rc = INTERNAL_ERROR;
            }
        }
    }

    if (SUCCEED(rc)) {
        if (mParm.info.enableLogo && ISNULL(mLogo)) {
            mLogo = Algorithm<BmpWaterMark>::create(
                "LogoWaterMark", "LogoWaterMarkThread", false, 1, 4,
                bmp,
                BMP_WATER_MARK_POSITION_BOTTOM_LEFT,
                ROTATION_ANGLE_0,
                20);
            if (ISNULL(mLogo)) {
                LOGE(mModule, "Failed to create bmp water mark algorithm");
                rc = INTERNAL_ERROR;
            }
        }
    }

    if (SUCCEED(rc)) {
        if (mParm.info.enableLogo && NOTNULL(mLogo)) {
            AlgTraits<BmpWaterMark>::ParmType parm;
            parm.rotation   = mParm.rotation;
            parm.dp         = mParm.info.dp;
            parm.dpi        = mParm.info.dpi;
            parm.needMirror = needMirror;
            parm.wmRatio    = mParm.info.wmRatio;
            parm.position   = BMP_WATER_MARK_POSITION_TOP_LEFT;
            parm.workmode   = WM_WORK_COVERAGE;
            rc = mLogo->config(parm);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to config %s, %d", mLogo->whoamI(), rc);
            }
        }
    }

    if (SUCCEED(rc)) {
        if (mParm.info.enableLogo && NOTNULL(mLogo)) {
            rc = mLogo->process(t);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to draw logo, %d", rc);
                rc = INTERNAL_ERROR;
            }
        }
    }

    if (SUCCEED(rc)) {
        if (mParm.info.enableTexts && ISNULL(mFont)) {
            char product[DEBUG_TEXT_MAX_LEN];
            snprintf(product, sizeof(product) - 1,
                "Product: %s", mParm.info.product);
            char build[DEBUG_TEXT_MAX_LEN];
            snprintf(build, sizeof(build) - 1,
                "Build: %s", mParm.info.build);
            char package[DEBUG_TEXT_MAX_LEN];
            snprintf(package, sizeof(package) - 1,
                "Package: %s", mParm.info.package);
            char debugInfo[DEBUG_TEXT_MAX_LEN];
            snprintf(debugInfo, sizeof(debugInfo) - 1,
                "%s Version.", mParm.info.debug ?
                "User Debug Build" : "User Build");

            mFont = Algorithm<FontWaterMark>::create(
                "FontWaterMark", "FontWaterMarkThread", false, 1, 2,
                FONT_WATER_MARK_POSITION_MIDDLE,
                ROTATION_ANGLE_0,
                "Powered by Pandora.", 20,
                product,               20,
                build,                 20,
                package,               20,
                debugInfo,             20,
                "STOP");
            if (ISNULL(mFont)) {
                LOGE(mModule, "Failed to create font water mark algorithm");
                rc = INTERNAL_ERROR;
            }
        }
    }

    if (SUCCEED(rc)) {
        if (mParm.info.enableTexts && NOTNULL(mFont)) {
            AlgTraits<FontWaterMark>::ParmType parm;
            parm.rotation   = mParm.rotation;
            parm.needMirror = needMirror;
            parm.position   = FONT_WATER_MARK_POSITION_BOTTOM_RIGHT;
            parm.texts      = NULL;
            rc = mFont->config(parm);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to config %s, %d", mFont->whoamI(), rc);
            }
        }
    }

    if (SUCCEED(rc)) {
        if (mParm.info.enableTexts && NOTNULL(mFont)) {
            rc = mFont->process(t);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to draw fonts, %d", rc);
                rc = INTERNAL_ERROR;
            }
        }
    }

    if (SUCCEED(rc)) {
        mDump ? dumpNV21ToFile(task.data, "water_mark_output",
            task.w, task.h, task.stride, task.scanline) : NO_ERROR;
        mPf ? PfLogger::getInstance()
            ->stop("water_mark_process") : NO_ERROR;

        if (task.type == FRAME_TYPE_SNAPSHOT) {
            LOGI(mModule, "Water mark added to snapshot.");
        }
    }
    dumpNV21ToJpeg(task.data, "water_mark_output",
            task.w, task.h, task.stride, task.scanline);

    if (SUCCEED(rc)) {
        result.valid = true;
    }

    return rc;
}

int32_t WaterMark::setParm(ParmTypeT &parm)
{
    mParm = parm;
    mParmValid = true;
    return NO_ERROR;
}

int32_t WaterMark::update(UpdateTypeT &/*update*/)
{
    // Don't care any updates from other algorithms
    return NO_ERROR;
}

int32_t WaterMark::queryCaps(AlgCaps &caps)
{
    caps.frameNeeded    =  SINGLE_FRAME_ALGORITHM_INPUT;
    caps.workingBufCnt  = 0;
    caps.workingBufSize = 0;
    caps.types   = FRAME_TYPE_MASK_PREVIEW |
        FRAME_TYPE_MASK_SNAPSHOT | FRAME_TYPE_MASK_VIDEO;
    caps.formats = FRAME_FORMAT_MASK_YUV_420_NV21 |
        FRAME_FORMAT_MASK_YUV_420_NV12 | FRAME_FORMAT_MASK_YUV_MONO;

    return NO_ERROR;
}

int32_t WaterMark::process(TaskTypeT &in, TaskTypeT &out, ResultTypeT &result)
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

int32_t WaterMark::queryStatus(StatusTypeT &/*status*/)
{
    // No status changed and not necessary to notify caller
    return NOT_SUPPORTED;
}

};


