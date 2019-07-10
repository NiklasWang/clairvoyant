#include <list>

#include "FontWaterMark.h"
#include "AlgorithmHelper.h"
#include "PfLogger.h"
#include "MemMgmt.h"

#include "YUVRotator.h"
#include "Algorithm.h"

namespace pandora {

INIT_ALGORITHM_TRAITS_FUNC(FontWaterMark);

#define AVERAGE_PERFORMANCE_COUNT 10
#define MSG_TERMINATOR_STRING     "STOP"

#define FONT_PATH                 "/system/fonts/ZUKChinese.ttf"

#define Y_TRANSPARENT_THRESHOLD   234
#define FILL_BLACK_RATIO          0.01 // 5% of input frame size

FontWaterMarkPosition gFontPositionMaps[][4] = {
    // 0                                  90                                    180                                  270
    {FONT_WATER_MARK_POSITION_TOP_LEFT,     FONT_WATER_MARK_POSITION_BOTTOM_LEFT,  FONT_WATER_MARK_POSITION_BOTTOM_RIGHT, FONT_WATER_MARK_POSITION_TOP_RIGHT    }, // position top left
    {FONT_WATER_MARK_POSITION_BOTTOM_LEFT,  FONT_WATER_MARK_POSITION_BOTTOM_RIGHT, FONT_WATER_MARK_POSITION_TOP_RIGHT,    FONT_WATER_MARK_POSITION_TOP_LEFT     }, // position bottom left
    {FONT_WATER_MARK_POSITION_BOTTOM_RIGHT, FONT_WATER_MARK_POSITION_TOP_RIGHT,    FONT_WATER_MARK_POSITION_TOP_LEFT,     FONT_WATER_MARK_POSITION_BOTTOM_LEFT  }, // position bottom right
    {FONT_WATER_MARK_POSITION_TOP_RIGHT,    FONT_WATER_MARK_POSITION_TOP_LEFT,     FONT_WATER_MARK_POSITION_BOTTOM_LEFT,  FONT_WATER_MARK_POSITION_BOTTOM_RIGHT }, // position top right
    {FONT_WATER_MARK_POSITION_MIDDLE,       FONT_WATER_MARK_POSITION_MIDDLE,       FONT_WATER_MARK_POSITION_MIDDLE,       FONT_WATER_MARK_POSITION_MIDDLE       }, // position middle
};

FontWaterMarkPosition gFMirrorPositionMaps[5] = {
    FONT_WATER_MARK_POSITION_TOP_RIGHT,     // position top left
    FONT_WATER_MARK_POSITION_BOTTOM_RIGHT,  // position bottom left
    FONT_WATER_MARK_POSITION_BOTTOM_LEFT,   // position bottom right
    FONT_WATER_MARK_POSITION_TOP_LEFT,      // position top right
    FONT_WATER_MARK_POSITION_MIDDLE,        // position middle
};


struct FontBlockCoordinate {
    int32_t startX;
    int32_t startY;
};

FontWaterMark::FontWaterMark(uint32_t argNum, va_list va) :
    mInited(false),
    mModule(MODULE_ALGORITHM),
    mPf(false),
    mPfCnt(AVERAGE_PERFORMANCE_COUNT),
    mDump(false),
    mFontMap(NULL),
    mFontYuv(NULL),
    mFontBlockXY(NULL),
    mYuvW(0),
    mYuvH(0),
    mRotator(NULL),
    mRotation(ROTATION_ANGLE_0),
    mPosition(FONT_WATER_MARK_POSITION_MAX_INVALID),
    mNeedMirror(false)
{
    bool stop = false;

    if (argNum == 2) {
        int32_t position = va_arg(va, int32_t);
        if (position >= 0 &&
            position < FONT_WATER_MARK_POSITION_MAX_INVALID) {
            mPosition = static_cast<FontWaterMarkPosition>(position);
        } else {
            LOGE(mModule, "Invalid position %d, reset to bottom right",
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
    }

    do {
        const char * str = reinterpret_cast<const char *>(va_arg(va, int32_t));
        if (COMPARE_SAME_STRING(str, MSG_TERMINATOR_STRING)) {
            stop = true;
        } else {
            mStrings.push_back(std::string(str));
            mSizes.push_back(va_arg(va, int32_t));
        }
    } while (!stop);
}

FontWaterMark::~FontWaterMark()
{
    if (mInited) {
        deinit();
    }
}

int32_t FontWaterMark::init()
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

int32_t FontWaterMark::deinit()
{
    int32_t rc = NO_ERROR;

    if (!mInited) {
        rc = NOT_INITED;
    }

    if (SUCCEED(rc)) {
        int32_t fontNum = getFontNum();
        rc = destroyBitmap(fontNum, mFontMap);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to destroy font bitmap, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        SECURE_FREE(mFontMap);
        SECURE_FREE(mFontYuv);
        SECURE_FREE(mFontBlockXY);
    }

    if (SUCCEED(rc)) {
        mInited = false;
    }

    return rc;
}

int32_t FontWaterMark::calcFontWaterMarkSize(int32_t *outputW, int32_t *outputH)
{
    int32_t w = 0;
    int32_t h = 0;
    for (uint32_t i = 0; i < mStrings.size(); i++) {
        int32_t fontSize = mSizes[i];
        int32_t lineX = mStrings[i].length() * fontSize;
        w = w < lineX ? lineX : w;
        h += fontSize;
    }
    *outputW= w;
    *outputH = h;

    return NO_ERROR;
}

int32_t FontWaterMark::calcFontBlockStartingCoordinates()
{
    int32_t index = 0;
    int32_t xSum = 0, ySum = 0;
    for (uint32_t i = 0; i < mStrings.size(); i++) {
        xSum = 0;
        int32_t fontSize = mSizes[i];
        for (uint32_t j = 0; j < mStrings[i].length(); j++) {
            mFontBlockXY[index].startX = xSum;
            mFontBlockXY[index].startY = ySum;
            xSum += fontSize;
            index++;
        }
        ySum += fontSize;
    }

    return NO_ERROR;
}

int32_t FontWaterMark::getFontNum()
{
    int32_t num = 0;

    for (uint32_t i = 0; i < mStrings.size(); i++) {
        num += mStrings[i].length();
    }

    return num;
}

int32_t FontWaterMark::buildFontMap()
{
    int32_t rc = NO_ERROR;
    int32_t fontNum = getFontNum();
    FT_Library lib;
    bool libInited = false;

    if (NOTNULL(mFontMap)) {
        rc = destroyBitmap(getFontNum(), mFontMap);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to destroy font bitmap, %d", rc);
        } else {
            SECURE_FREE(mFontMap);
        }
    }

    if (SUCCEED(rc)) {
        int32_t size = fontNum * sizeof(font_bitmap_t);
        mFontMap = (font_bitmap_t *)Malloc(size);
        if (ISNULL(mFontMap)) {
            LOGE(mModule, "Failed to alloc %d bytes for "
                "bitmap header, %d", size, rc);
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        rc = initLib(&lib);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to init ft lib, %d", rc);
        } else {
            libInited = true;
        }
    }

    if (SUCCEED(rc)) {
        int32_t index = 0;
        for (uint32_t i = 0; i < mStrings.size(); i++) {
            for (uint32_t j = 0; j < mStrings[i].length(); j++) {
                rc = getBitmap(&lib, mStrings[i][j],
                    mSizes[i], FONT_PATH, &mFontMap[index]);
                if (!SUCCEED(rc)) {
                    LOGE(mModule, "Failed to get bitmap, %d", rc);
                } else {
                    index++;
                }
            }
        }
    }

    if (SUCCEED(rc) || !SUCCEED(rc)) {
        if (libInited) {
            rc = deinitLib(&lib);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to deinit lib, %d", rc);
            }
        }
    }

    return rc;
}

int32_t FontWaterMark::buildFontYuv(int32_t yuvW, int32_t yuvH)
{
    int32_t rc = NO_ERROR;
    bool build = false;

    if (SUCCEED(rc)) {
        if (NOTNULL(mFontYuv)) {
            SECURE_FREE(mFontYuv);
            mYuvW = mYuvH = 0;
        }
    }

    if (SUCCEED(rc) && ISNULL(mFontYuv)) {
        size_t size = getFrameLen2(yuvW, yuvH, 2, 2);
        mFontYuv = (uint8_t *)Malloc(size);
        if (ISNULL(mFontYuv)) {
            LOGE(mModule, "Failed to alloc %d bytes memory", size);
            rc = NO_MEMORY;
        } else {
            memset(mFontYuv, 0, size);
            mYuvW = yuvW;
            mYuvH = yuvH;
            build = true;
        }
    }

    if (SUCCEED(rc) && build) {
        int32_t yuvX = 0, yuvY = 0;
        uint8_t *yPlane = getNV21YData(mFontYuv, mYuvW, mYuvH, 2, 2);
        uint8_t *uvPlane = getNV21UVData(mFontYuv, mYuvW, mYuvH, 2, 2);
        uint32_t ylen = getAlignedStride(mYuvW, 2);
        uint32_t uvlen = getAlignedStride(mYuvW, 2);
        for ( int32_t fontIndex = 0; fontIndex < getFontNum(); fontIndex++) {
            int32_t fontWith = mFontMap[fontIndex].pitch;
            int32_t fontHeight = mFontMap[fontIndex].h;
            for (int32_t x = 0; x < fontWith; x++) {
                for (int32_t y = 0; y < fontHeight; y++) {
                    yuvX = x + mFontBlockXY[fontIndex].startX;
                    yuvY = y + mFontBlockXY[fontIndex].startY;
                    if (*(mFontMap[fontIndex].buffer + y * fontWith + x)) {
                        yPlane[yuvY * ylen + yuvX] = 0xFF;
                        if (yuvY % 2 == 0)
                            uvPlane[yuvY / 2 * uvlen + yuvX] = 0x80;
                    }
                }
            }
        }
    }

    return rc;
}

int32_t calcStartXY(int32_t w, int32_t h, int32_t fontYuvW, int32_t fontYuvH,
    int32_t *startX, int32_t *startY, FontWaterMarkPosition position)
{
    int32_t x, y;

    switch (position) {
        case FONT_WATER_MARK_POSITION_MIDDLE: {
            x = w / 2 - fontYuvW / 2;
            y = h / 2 - fontYuvH / 2;
        } break;
        case FONT_WATER_MARK_POSITION_TOP_LEFT: {
            x = 0;
            y = 0;
            x += FILL_BLACK_RATIO * w;
            y += FILL_BLACK_RATIO * h;
        } break;
        case FONT_WATER_MARK_POSITION_TOP_RIGHT: {
            x = w - fontYuvW;
            y = 0;
            x -= FILL_BLACK_RATIO * w;
            y += FILL_BLACK_RATIO * h;
        } break;
        case FONT_WATER_MARK_POSITION_BOTTOM_LEFT: {
            x = 0;
            y = h - fontYuvH;
            x += FILL_BLACK_RATIO * w;
            y -= FILL_BLACK_RATIO * h;
        } break;
        case FONT_WATER_MARK_POSITION_BOTTOM_RIGHT:
        default: {
            x = w - fontYuvW;
            y = h - fontYuvH;
            x -= FILL_BLACK_RATIO * w;
            y -= FILL_BLACK_RATIO * h;
        }
    }

    if (x + fontYuvW > w) {
        x = w - fontYuvW;
    }

    if (y + fontYuvH > h) {
        y = h - fontYuvH;
    }

    x = x > 0 ? x : 0;
    y = y > 0 ? y : 0;

    *startX = x;
    *startY = y;

    return NO_ERROR;
}

int32_t FontWaterMark::process(TaskTypeT &task, ResultTypeT &result)
{
    int32_t rc = NO_ERROR;
    uint8_t *frame = NULL;
    int32_t startX = 0, startY = 0;
    int32_t yuvW = 0, yuvH = 0;
    int32_t fontNum = getFontNum();
    RotationAngle         rotation;
    FontWaterMarkPosition position = FONT_WATER_MARK_POSITION_TOP_LEFT;

    if (!mInited) {
        rc = NOT_INITED;
    } else {
        frame = (uint8_t *)task.data;
        result.valid = false;
        result.taskid = task.taskid;
    }

    if (SUCCEED(rc)) {
        if (ISNULL(mFontBlockXY)) {
            int32_t size = fontNum * sizeof(FontBlockCoordinate);
            mFontBlockXY = (FontBlockCoordinate *)Malloc(size);
            if (ISNULL(mFontBlockXY)) {
                LOGE(mModule, "Failed to alloc %d bytes for "
                        "font block coordinate, %d", size, rc);
                rc = NO_MEMORY;
            }
        }
    }

    if (SUCCEED(rc)) {
        if (ISNULL(mFontMap)) {
            rc = buildFontMap();
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to rebuild font bitmap, %d", rc);
            }
        }
    }

    if (SUCCEED(rc)) {
        rc = calcFontWaterMarkSize(&yuvW, &yuvH);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to get water mark size, %d", rc);
        } else {
            LOGD(mModule, "Font Wanter Mark Yuv width %d, "
                "height %d", yuvW, yuvH);
        }
    }

    if (SUCCEED(rc)) {
        memset(mFontBlockXY, 0, (fontNum * sizeof(FontBlockCoordinate)));
        rc = calcFontBlockStartingCoordinates();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to get font block"
                "starting coordinates, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = buildFontYuv(yuvW, yuvH);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to build yuv image, %d", rc);
        } else {
            mYuvW = yuvW;
            mYuvH = yuvH;
            rotation = mRotation;
        }
    }

    if (SUCCEED(rc)) {
        if (mNeedMirror) {
            uint8_t *y1 = getNV21YData(mFontYuv, mYuvW, mYuvH, 2, 2);
            uint8_t *uv1 = getNV21UVData(mFontYuv, mYuvW, mYuvH, 2, 2);

            for (int32_t j = 0; j < mYuvH; j++) {
                for (int32_t i = 0; i < (mYuvW / 2); i++) {
                    uint8_t tempY = y1[j * mYuvW + i];
                    y1[j * mYuvW + i] = y1[j * mYuvW + mYuvW - i - 1];
                    y1[j * mYuvW + mYuvW - i - 1] = tempY;
                    if ((j % 2) == 0 && (i % 2) == 0) {
                        uint8_t tempUV = uv1[(j / 2) * mYuvW + i];
                        uv1[(j / 2) * mYuvW + i] = uv1[(j / 2) * mYuvW +  mYuvW - i - 2];
                        uv1[(j / 2) * mYuvW +  mYuvW - i - 2] = tempUV;
                        tempUV = uv1[(j / 2) * mYuvW + i + 1];
                        uv1[(j / 2) * mYuvW + i + 1] = uv1[(j / 2) * mYuvW +  mYuvW - i - 1];
                        uv1[(j / 2) * mYuvW +  mYuvW - i - 1] = tempUV;
                    }
                }
            }
        }
    }


    if (SUCCEED(rc)) {
        if (mPosition >= FONT_WATER_MARK_POSITION_MAX_INVALID ||
            mRotation < ROTATION_ANGLE_0 || mRotation > ROTATION_ANGLE_270) {
            LOGE(mModule, "rotation angle %d or position %d is not right",
                mRotation, mPosition);
            rc = PARAM_INVALID;
        } else {
            position = mPosition;
            if (mNeedMirror) {
                position = gFMirrorPositionMaps[position];
            }
            position = gFontPositionMaps[position][mRotation];
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
                    LOGE(mModule, "Failed to config rotator, %d", rc);
                }
            }
        }

        if (SUCCEED(rc)) {
            GlobalTaskType t(mFontYuv, mYuvW, mYuvH, 2, 2,
                getFrameLen2(mYuvW, mYuvH, 2, 2),
                FRAME_FORMAT_YUV_420_NV21, FRAME_TYPE_SNAPSHOT);
            rc = mRotator->process(t);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to rotate frame %d, %d", mRotation, rc);
                rc = INTERNAL_ERROR;
            } else {
                if (mRotation == ROTATION_ANGLE_90 ||
                    mRotation == ROTATION_ANGLE_270) {
                    int32_t tmp = mYuvW;
                    mYuvW = mYuvH;
                    mYuvH = tmp;
                }
            }
        }
    }

    if (SUCCEED(rc)) {
        rc = calcStartXY(task.w, task.h, mYuvW, mYuvH,
            &startX, &startY, position);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to calc start x y postion, %d", rc);
        } else {
            LOGD(mModule, "Font Wanter Mark Yuv startX %d, "
                "startY %d", startX, startY);
        }
    }

    if (SUCCEED(rc)) {
        mDump ? dumpNV21ToFile(mFontYuv, "font_water_mark",
            mYuvW, mYuvH, 2, 2) : NO_ERROR;
        mPf ? PfLogger::getInstance()
            ->start("font_water_mark_process", mPfCnt) : NO_ERROR;
    }

    if (SUCCEED(rc)) {
        uint8_t *y1 = getNV21YData((uint8_t *)task.data,
            task.w, task.h, task.stride, task.scanline);
        uint8_t *uv1 = getNV21UVData((uint8_t *)task.data,
            task.w, task.h, task.stride, task.scanline);
        uint32_t ylen1 = getAlignedStride(task.w, task.stride);
        uint32_t uvlen1 = getAlignedStride(task.w, task.stride);
        uint8_t *y2 = getNV21YData(mFontYuv, mYuvW, mYuvH, 2, 2);
        uint8_t *uv2 = getNV21UVData(mFontYuv, mYuvW, mYuvH, 2, 2);
        for (int32_t i = 0; i < mYuvW; i++) {
            for (int32_t j = 0; j < mYuvH; j++) {
                if (i + startX < task.w && j + startY < task.h &&
                    mFontYuv[j * mYuvW + i] == 0xFF) {
                    y1[(j + startY) * ylen1 + i + startX] = y2[j * mYuvW + i];
                    if ((j % 2) == 0) {
                        uv1[(j + startY) / 2 * uvlen1 + i + startX] = uv2[j / 2 * mYuvW + i];
                    }
                }
            }
        }
    }

    if (SUCCEED(rc)) {
        mDump ? dumpNV21ToFile(frame, "image_font_water_mark",
            task.w, task.h, task.stride, task.scanline) : NO_ERROR;
        mPf ? PfLogger::getInstance()
            ->stop("font_water_mark_process") : NO_ERROR;
    }

    if (SUCCEED(rc)) {
        result.valid = true;
    }

    return rc;
}

int32_t FontWaterMark::setParm(ParmTypeT &parm)
{
    int32_t rc = NO_ERROR;
    mNeedMirror = parm.needMirror;

    if (parm.rotation >= ROTATION_ANGLE_0 &&
        parm.rotation < ROTATION_ANGLE_MAX_INVALID) {
        mRotation = parm.rotation;
    }

    if (parm.position != FONT_WATER_MARK_POSITION_MAX_INVALID) {
        mPosition = parm.position;
    }

    if (NOTNULL(parm.texts)) {
        std::vector<std::string> strings;
        std::vector<int32_t>     sizes;
        for (auto iter : *parm.texts) {
            strings.push_back(iter.str);
            sizes.push_back(iter.size);
        }

        bool update = false;
        if (sizes.size() != mSizes.size() ||
            strings.size() != mStrings.size()) {
            update = true;
        }

        if (!update) {
            uint32_t i = 0;
            for (auto iter = mStrings.begin();
                iter != mStrings.end(); iter++, i++) {
                if (*iter != strings[i]) {
                    update = true;
                    break;
                }
            }
        }

        if (!update) {
            uint32_t i = 0;
            for (auto iter = mSizes.begin();
                iter != mSizes.end(); iter++, i++) {
                if (*iter != sizes[i]) {
                    update = true;
                    break;
                }
            }
        }

        if (update) {
            rc = deinit();
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to deinit water mark "
                    "when set parm, %d", rc);
            }

            rc = init();
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to init water mark "
                    "when set parm, %d", rc);
            }

            mStrings = strings;
            mSizes   = sizes;
        }
    }

    return rc;
}

int32_t FontWaterMark::update(UpdateTypeT &/*update*/)
{
    // Don't care any updates from other algorithms
    return NO_ERROR;
}

int32_t FontWaterMark::queryCaps(AlgCaps &caps)
{
    caps.frameNeeded    =  SINGLE_FRAME_ALGORITHM_INPUT;
    caps.workingBufCnt  = 0;
    caps.workingBufSize = 0;
    caps.types   = FRAME_TYPE_MASK_PREVIEW |
        FRAME_TYPE_MASK_SNAPSHOT | FRAME_TYPE_MASK_VIDEO;
    caps.formats = FRAME_FORMAT_MASK_YUV_420_NV21 | FRAME_FORMAT_MASK_YUV_420_NV12;

    return NO_ERROR;
}

int32_t FontWaterMark::process(TaskTypeT &in, TaskTypeT &out, ResultTypeT &result)
{
    int32_t rc = NO_ERROR;
    uint8_t *frame = NULL;
    int32_t startX = 0, startY = 0;
    int32_t yuvW = 0, yuvH = 0;
    int32_t fontNum = getFontNum();
    RotationAngle         rotation;
    FontWaterMarkPosition position = FONT_WATER_MARK_POSITION_TOP_LEFT;

    if (!mInited) {
        rc = NOT_INITED;
    } else {
        frame = (uint8_t *)out.data;
        result.valid = false;
        result.taskid = in.taskid;
    }

    if (SUCCEED(rc)) {
        void *data = out.data;
        out = in;
        memcpy(data, in.data, in.size);
        out.data = data;
    }

    if (SUCCEED(rc)) {
        if (ISNULL(mFontBlockXY)) {
            int32_t size = fontNum * sizeof(FontBlockCoordinate);
            mFontBlockXY = (FontBlockCoordinate *)Malloc(size);
            if (ISNULL(mFontBlockXY)) {
                LOGE(mModule, "Failed to alloc %d bytes for "
                        "font block coordinate, %d", size, rc);
                rc = NO_MEMORY;
            }
        }
    }

    if (SUCCEED(rc)) {
        if (ISNULL(mFontMap)) {
            rc = buildFontMap();
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to rebuild font bitmap, %d", rc);
            }
        }
    }

    if (SUCCEED(rc)) {
        rc = calcFontWaterMarkSize(&yuvW, &yuvH);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to get water mark size, %d", rc);
        } else {
            LOGD(mModule, "Font Wanter Mark Yuv width %d, "
                "height %d", yuvW, yuvH);
        }
    }

    if (SUCCEED(rc)) {
        memset(mFontBlockXY, 0, (fontNum * sizeof(FontBlockCoordinate)));
        rc = calcFontBlockStartingCoordinates();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to get font block"
                "starting coordinates, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = buildFontYuv(yuvW, yuvH);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to build yuv image, %d", rc);
        } else {
            mYuvW = yuvW;
            mYuvH = yuvH;
            rotation = mRotation;
        }
    }

    if (SUCCEED(rc)) {
        if (mNeedMirror) {
            uint8_t *y1 = getNV21YData(mFontYuv, mYuvW, mYuvH, 2, 2);
            uint8_t *uv1 = getNV21UVData(mFontYuv, mYuvW, mYuvH, 2, 2);

            for (int32_t j = 0; j < mYuvH; j++) {
                for (int32_t i = 0; i < (mYuvW / 2); i++) {
                    uint8_t tempY = y1[j * mYuvW + i];
                    y1[j * mYuvW + i] = y1[j * mYuvW + mYuvW - i - 1];
                    y1[j * mYuvW + mYuvW - i - 1] = tempY;
                    if ((j % 2) == 0 && (i % 2) == 0) {
                        uint8_t tempUV = uv1[(j / 2) * mYuvW + i];
                        uv1[(j / 2) * mYuvW + i] = uv1[(j / 2) * mYuvW +  mYuvW - i - 2];
                        uv1[(j / 2) * mYuvW +  mYuvW - i - 2] = tempUV;
                        tempUV = uv1[(j / 2) * mYuvW + i + 1];
                        uv1[(j / 2) * mYuvW + i + 1] = uv1[(j / 2) * mYuvW +  mYuvW - i - 1];
                        uv1[(j / 2) * mYuvW +  mYuvW - i - 1] = tempUV;
                    }
                }
            }
        }
    }


    if (SUCCEED(rc)) {
        if (mPosition >= FONT_WATER_MARK_POSITION_MAX_INVALID ||
            mRotation < ROTATION_ANGLE_0 || mRotation > ROTATION_ANGLE_270) {
            LOGE(mModule, "rotation angle %d or position %d is not right",
                mRotation, mPosition);
            rc = PARAM_INVALID;
        } else {
            position = mPosition;
            if (mNeedMirror) {
                position = gFMirrorPositionMaps[position];
            }
            position = gFontPositionMaps[position][mRotation];
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
                    LOGE(mModule, "Failed to config rotator, %d", rc);
                }
            }
        }

        if (SUCCEED(rc)) {
            GlobalTaskType t(mFontYuv, mYuvW, mYuvH, 2, 2,
                getFrameLen2(mYuvW, mYuvH, 2, 2),
                FRAME_FORMAT_YUV_420_NV21, FRAME_TYPE_SNAPSHOT);
            rc = mRotator->process(t);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to rotate frame %d, %d", mRotation, rc);
                rc = INTERNAL_ERROR;
            } else {
                if (mRotation == ROTATION_ANGLE_90 ||
                    mRotation == ROTATION_ANGLE_270) {
                    int32_t tmp = mYuvW;
                    mYuvW = mYuvH;
                    mYuvH = tmp;
                }
            }
        }
    }

    if (SUCCEED(rc)) {
        rc = calcStartXY(in.w, in.h, mYuvW, mYuvH,
            &startX, &startY, position);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to calc start x y postion, %d", rc);
        } else {
            LOGD(mModule, "Font Wanter Mark Yuv startX %d, "
                "startY %d", startX, startY);
        }
    }

    if (SUCCEED(rc)) {
        mDump ? dumpNV21ToFile(mFontYuv, "font_water_mark",
            mYuvW, mYuvH, 2, 2) : NO_ERROR;
        mPf ? PfLogger::getInstance()
            ->start("font_water_mark_process", mPfCnt) : NO_ERROR;
    }

    if (SUCCEED(rc)) {
        uint8_t *yOut = getNV21YData((uint8_t *)out.data,
            out.w, out.h, out.stride, out.scanline);
        uint8_t *uvOut = getNV21UVData((uint8_t *)out.data,
            out.w, out.h, out.stride, out.scanline);
        uint32_t yOutLen = getAlignedStride(out.w, out.stride);
        uint32_t uvOutLen = getAlignedStride(out.w, out.stride);

        uint8_t *yFont = getNV21YData(mFontYuv, mYuvW, mYuvH, 2, 2);
        uint8_t *uvFont = getNV21UVData(mFontYuv, mYuvW, mYuvH, 2, 2);
        for (int32_t i = 0; i < mYuvW; i++) {
            for (int32_t j = 0; j < mYuvH; j++) {
                if (i + startX < out.w && j + startY < out.h &&
                    mFontYuv[j * mYuvW + i] == 0xFF) {
                    yOut[(j + startY) * yOutLen + i + startX] = yFont[j * mYuvW + i];
                    if ((j % 2) == 0) {
                        uvOut[(j + startY) / 2 * uvOutLen + i + startX] = uvFont[j / 2 * mYuvW + i];
                    }
                }
            }
        }
    }

    if (SUCCEED(rc)) {
        mDump ? dumpNV21ToFile(frame, "image_font_water_mark",
            out.w, out.h, out.stride, out.scanline) : NO_ERROR;
        mPf ? PfLogger::getInstance()
            ->stop("font_water_mark_process") : NO_ERROR;
    }

    if (SUCCEED(rc)) {
        result.valid = true;
    }

    return rc;
}

#ifndef __FUNCTION_TEST_

int32_t FontWaterMark::queryStatus(StatusTypeT &/*status*/)
{
    // No status changed and not necessary to notify caller
    return NOT_SUPPORTED;
}

#else

int32_t FontWaterMark::queryStatus(StatusTypeT &status)
{
    static int32_t runTime = 0;
    status.rc = runTime++ ? NO_ERROR : NOT_SUPPORTED;

    if (runTime == 2) {
        runTime = 0;
    }

    return NO_ERROR;
}

int32_t FontWaterMark::testNewInterface(int32_t rc)
{
    return rc;
}

#endif


};

