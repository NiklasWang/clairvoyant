#include "Common.h"
#include "FrameCounter.h"

namespace pandora {

FrameCounter::FrameCounter() :
     mModule(MODULE_UTILS),
     mWorkMode(SingleCameraMode)
{
    memset(&mCounter, 0, sizeof(mCounter));
    pthread_mutex_init(&mMutex, NULL);
}

int32_t FrameCounter::reset()
{
    memset(&mCounter, 0, sizeof(mCounter));

    return NO_ERROR;
}

int32_t FrameCounter::setCameraMode(CameraMode mode)
{
    int32_t rc = NO_ERROR;

    mWorkMode = mode;
    LOGD(mModule, "This snapshot mode is %s",
        mWorkMode == SingleCameraMode ?
        "single camera mode" : "dual camera mode");

    return rc;
}

int32_t FrameCounter::setTotalNum(int32_t num)
{
    int32_t rc = NO_ERROR;

    if (mWorkMode == DualCameraMode) {
        mCounter.mDualCounter.mTotalPairs = num;
    } else {
        mCounter.mSingleCounter.mTotalNum = num;
    }

    return rc;
}

int32_t FrameCounter::countFrame(FrameType type, FrameFormat format, bool *isTargetFrame)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (type != FRAME_TYPE_SNAPSHOT) {
            rc = NOT_NEEDED;
        }
    }

    if (SUCCEED(rc)) {
        if (format != FRAME_FORMAT_YUV_420_NV21 &&
            format != FRAME_FORMAT_YUV_420_NV12 &&
            format != FRAME_FORMAT_YUV_MONO) {
            rc = NOT_NEEDED;
        }
    }

    if (SUCCEED(rc)) {
        *isTargetFrame = false;
    }

    if (SUCCEED(rc)) {
        if (mWorkMode == SingleCameraMode) {
            mCounter.mSingleCounter.mFrameCounter++;
            if (mCounter.mSingleCounter.mFrameCounter ==
                mCounter.mSingleCounter.mTotalNum) {
                *isTargetFrame = true;
            }

            if (mCounter.mSingleCounter.mFrameCounter >
                mCounter.mSingleCounter.mTotalNum) {
                LOGE(mModule, "FrameNeeded %d is not right in single "
                    "camera mode", mCounter.mSingleCounter.mTotalNum);
                rc = PARAM_INVALID;
            }
        }
    }

    if (SUCCEED(rc)) {
        if (mWorkMode == DualCameraMode) {
            if (mCounter.mDualCounter.mPairCounter ==
                    (mCounter.mDualCounter.mTotalPairs - 1) &&
                (format == FRAME_FORMAT_YUV_420_NV21 ||
                    format == FRAME_FORMAT_YUV_420_NV12)) {
                *isTargetFrame = true;
            }

            if (format == FRAME_FORMAT_YUV_420_NV21 ||
                format == FRAME_FORMAT_YUV_420_NV12) {
                mCounter.mDualCounter.mColorArrived = true;
            }

            if (format == FRAME_FORMAT_YUV_MONO) {
                mCounter.mDualCounter.mMonoArrived = true;
            }

            pthread_mutex_lock(&mMutex);
            if (mCounter.mDualCounter.mColorArrived &&
                mCounter.mDualCounter.mMonoArrived) {
                mCounter.mDualCounter.mPairCounter++;
                mCounter.mDualCounter.mColorArrived = false;
                mCounter.mDualCounter.mMonoArrived = false;
            }
            pthread_mutex_unlock(&mMutex);

            if (mCounter.mDualCounter.mPairCounter >
                mCounter.mDualCounter.mTotalPairs) {
                LOGE(mModule, "FrameNeeded %d is not right in dual "
                    "camera mode", mCounter.mDualCounter.mTotalPairs);
                rc = PARAM_INVALID;
            }
        }
    }

    return RETURNIGNORE(rc, NOT_NEEDED);
}

int32_t FrameCounter::getCounter()
{
    int32_t result = 1;

    if (mWorkMode == SingleCameraMode) {
        result = mCounter.mSingleCounter.mFrameCounter;
    } else if (mWorkMode == DualCameraMode) {
        result = mCounter.mDualCounter.mPairCounter;
    }

    return result;
}

bool FrameCounter::checkTarget()
{
    bool result = false;

    if (mWorkMode == SingleCameraMode) {
        if (mCounter.mSingleCounter.mFrameCounter ==
            mCounter.mSingleCounter.mTotalNum) {
            result = true;
        }
    } else if (mWorkMode == DualCameraMode) {
        if (mCounter.mDualCounter.mPairCounter ==
            mCounter.mDualCounter.mTotalPairs) {
            result = true;
        }
    }

    return result;
}

FrameCounter::~FrameCounter()
{
    pthread_mutex_destroy(&mMutex);
}

};
