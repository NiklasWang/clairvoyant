#include "Common.h"
#include "MemMgmt.h"
#include "FrameReorder.h"

namespace pandora {

#define WAIT_UNTIL_READY  0
#define DO_NOT_WAIT      -1

const ParmMap<FrameType, FrameFormat> FrameReorder::gOrders[] = {
    {
        .key = FRAME_TYPE_PREVIEW,
        .input1 = FRAME_FORMAT_YUV_MONO,
        .input2 = FRAME_FORMAT_YUV_420_NV21,
        .inputTimeout = DO_NOT_WAIT,
        .output1 = FRAME_FORMAT_YUV_MONO,
        .output2 = FRAME_FORMAT_YUV_420_NV21,
        .outputTimeout = DO_NOT_WAIT,
    },
    {
        .key = FRAME_TYPE_SNAPSHOT,
        .input1 = FRAME_FORMAT_YUV_MONO,
        .input2 = FRAME_FORMAT_YUV_420_NV21,
        .inputTimeout = WAIT_UNTIL_READY,
        .output1 = FRAME_FORMAT_YUV_420_NV21,
        .output2 = FRAME_FORMAT_YUV_MONO,
        .outputTimeout = 6000,
    },
    {
        .key = FRAME_TYPE_SNAPSHOT,
        .input1 = FRAME_FORMAT_JPEG_MONO,
        .input2 = FRAME_FORMAT_JPEG,
        .inputTimeout = DO_NOT_WAIT,
        .output1 = FRAME_FORMAT_JPEG_MONO,
        .output2 = FRAME_FORMAT_JPEG,
        .outputTimeout = DO_NOT_WAIT,
    },
    {
        .key = FRAME_TYPE_VIDEO,
        .input1 = FRAME_FORMAT_YUV_MONO,
        .input2 = FRAME_FORMAT_YUV_420_NV12,
        .inputTimeout = DO_NOT_WAIT,
        .output1 = FRAME_FORMAT_YUV_MONO,
        .output2 = FRAME_FORMAT_YUV_420_NV12,
        .outputTimeout = DO_NOT_WAIT,
    },
    {
        .key = FRAME_TYPE_PREVIEW,
        .input1 = FRAME_FORMAT_YUV_TELE,
        .input2 = FRAME_FORMAT_YUV_420_NV21,
        .inputTimeout = DO_NOT_WAIT,
        .output1 = FRAME_FORMAT_YUV_MONO,
        .output2 = FRAME_FORMAT_YUV_420_NV12,
        .outputTimeout = DO_NOT_WAIT,
    },
    {
        .key = FRAME_TYPE_SNAPSHOT,
        .input1 = FRAME_FORMAT_YUV_TELE,
        .input2 = FRAME_FORMAT_YUV_420_NV21,
        .inputTimeout = WAIT_UNTIL_READY,
        .output1 = FRAME_FORMAT_YUV_TELE,
        .output2 = FRAME_FORMAT_YUV_420_NV21,
        .outputTimeout = DO_NOT_WAIT,
    },
    {
        .key = FRAME_TYPE_SNAPSHOT,
        .input1 = FRAME_FORMAT_JPEG_TELE,
        .input2 = FRAME_FORMAT_JPEG,
        .inputTimeout = DO_NOT_WAIT,
        .output1 = FRAME_FORMAT_JPEG_TELE,
        .output2 = FRAME_FORMAT_JPEG,
        .outputTimeout = DO_NOT_WAIT,
    },
    {
        .key = FRAME_TYPE_VIDEO,
        .input1 = FRAME_FORMAT_YUV_TELE,
        .input2 = FRAME_FORMAT_YUV_420_NV12,
        .inputTimeout = DO_NOT_WAIT,
        .output1 = FRAME_FORMAT_YUV_TELE,
        .output2 = FRAME_FORMAT_YUV_420_NV12,
        .outputTimeout = DO_NOT_WAIT,
    },
};

int32_t FrameReorder::wait(FrameType type, FrameFormat format)
{
    int32_t rc = NO_ERROR;
    uint32_t index = 0;

    if (mStoped[type] ||
        format == FRAME_FORMAT_MAX_INVALID) {
        // reorder stopped or reorder not required
        rc = NOT_REQUIRED;
    }

    if (SUCCEED(rc)) {
        uint32_t i;
        for (i = 0; i < ARRAYSIZE(gOrders); i++) {
            if (type == gOrders[i].key) {
                if (format == gOrders[i].input1 ||
                    format == gOrders[i].input2) {
                    index = i;
                    break;
                }
            }
        }
        if (i == ARRAYSIZE(gOrders)) {
            rc = NOT_FOUND;
        }
    }

    if (SUCCEED(rc)) {
        if (gOrders[index].inputTimeout == DO_NOT_WAIT) {
            rc = NOT_REQUIRED;
        }
    }

    if (SUCCEED(rc)) {
        if (ISNULL(mOrderSems[index])) {
            mOrderSems[index] = new SemaphoreTimeout(
                gOrders[index].inputTimeout);
            if (ISNULL(mOrderSems[index])) {
                LOGE(mModule, "Failed to alloc for semaphoret.");
                rc = NO_MEMORY;
            }
        }
    }

    if (SUCCEED(rc)) {
        if (format == gOrders[index].input2) {
            bool needWait = mOrderSems[index]->needWait();
            if (needWait) {
                LOGI(mModule, "Wait frame, current use case %s "
                    "frame order %s -> %s, wait for %s",
                    frameTypeName(type),
                    frameFormatName(gOrders[index].input1),
                    frameFormatName(gOrders[index].input2),
                    frameFormatName(gOrders[index].input1));
            } else {
                LOGI(mModule, "Wait frame, current use case %s "
                    "frame order %s -> %s correct, no need to wait.",
                    frameTypeName(type),
                    frameFormatName(gOrders[index].input1),
                    frameFormatName(gOrders[index].input2));
            }
            rc = mOrderSems[index]->wait();
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to wait, inputTimeout %d ms",
                    gOrders[index].inputTimeout);
            } else {
                if (needWait) {
                    LOGD(mModule, "Wait frame succeed, current use case %s "
                        "frame order %s -> %s, received %s.",
                        frameTypeName(type),
                        frameFormatName(gOrders[index].output1),
                        frameFormatName(gOrders[index].output2),
                        frameFormatName(gOrders[index].output1));
                }
            }
        }
    }

    return RETURNIGNORE(rc, NOT_REQUIRED);
}

int32_t FrameReorder::signal(FrameType type, FrameFormat format)
{
    int32_t rc = NO_ERROR;
    uint32_t index = 0;

    if (mStoped[type] ||
        format == FRAME_FORMAT_MAX_INVALID) {
        // reorder stopped or reorder not required
        rc = NOT_REQUIRED;
    }

    if (SUCCEED(rc)) {
        uint32_t i;
        for (i = 0; i < ARRAYSIZE(gOrders); i++) {
            if (type == gOrders[i].key) {
                if (format == gOrders[i].input1 ||
                    format == gOrders[i].input2) {
                    index = i;
                    break;
                }
            }
        }
        if (i == ARRAYSIZE(gOrders)) {
            rc = NOT_FOUND;
        }
    }

    if (SUCCEED(rc)) {
        if (gOrders[index].inputTimeout == DO_NOT_WAIT) {
            rc = NOT_REQUIRED;
        }
    }

    if (SUCCEED(rc)) {
        if (ISNULL(mOrderSems[index])) {
            mOrderSems[index] = new SemaphoreTimeout(
                gOrders[index].inputTimeout);
            if (ISNULL(mOrderSems[index])) {
                LOGE(mModule, "Failed to alloc for semaphoret.");
                rc = NO_MEMORY;
            }
        }
    }

    if (SUCCEED(rc)) {
        if (format == gOrders[index].input1) {
            LOGI(mModule, "Signal frame, current use case %s,"
                "frame order %s -> %s, received %s, send signal.",
                frameTypeName(type),
                frameFormatName(gOrders[index].input1),
                frameFormatName(gOrders[index].input2),
                frameFormatName(gOrders[index].input1));
            mOrderSems[index]->signal();
        }
    }

    return RETURNIGNORE(rc, NOT_REQUIRED);
}


int32_t FrameReorder::reorder(FrameType type, FrameFormat format)
{
    int32_t rc = NO_ERROR;
    uint32_t index = 0;

    signal(type, format);

    if (mStoped[type] ||
        format == FRAME_FORMAT_MAX_INVALID) {
        // reorder stopped or reorder not required
        rc = NOT_REQUIRED;
    }

    if (SUCCEED(rc)) {
        uint32_t i;
        for (i = 0; i < ARRAYSIZE(gOrders); i++) {
            if (type == gOrders[i].key) {
                if (format == gOrders[i].output1 ||
                    format == gOrders[i].output2) {
                    index = i;
                    break;
                }
            }
        }
        if (i == ARRAYSIZE(gOrders)) {
            rc = NOT_FOUND;
        }
    }

    if (SUCCEED(rc)) {
        if (gOrders[index].outputTimeout == DO_NOT_WAIT) {
            rc = NOT_REQUIRED;
        }
    }

    if (SUCCEED(rc)) {
        if (ISNULL(mWaitSems[index])) {
            mWaitSems[index] = new SemaphoreTimeout(
                gOrders[index].outputTimeout);
            if (ISNULL(mWaitSems[index])) {
                LOGE(mModule, "Failed to alloc for semaphoret.");
                rc = NO_MEMORY;
            }
        }
    }

    if (SUCCEED(rc)) {
        if (format == gOrders[index].output1) {
            LOGI(mModule, "Signal reorder frame, current use case %s,"
                "frame order %s -> %s, received %s, send signal.",
                frameTypeName(type),
                frameFormatName(gOrders[index].output1),
                frameFormatName(gOrders[index].output2),
                frameFormatName(gOrders[index].output1));
            mWaitSems[index]->signal();
        }
        if (format == gOrders[index].output2) {
            bool needWait = mWaitSems[index]->needWait();
            if (needWait) {
                LOGI(mModule, "Reorder frame, "
                    "current use case %s received %s and wait for %s",
                    frameTypeName(type),
                    frameFormatName(gOrders[index].output2),
                    frameFormatName(gOrders[index].output1));
            } else {
                LOGI(mModule, "Reorder frame, current use case %s "
                    "frame order %s -> %s correct, no need to wait.",
                    frameTypeName(type),
                    frameFormatName(gOrders[index].output1),
                    frameFormatName(gOrders[index].output2));
            }
            rc = mWaitSems[index]->wait();
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to wait, waitted timeout %d ms",
                    gOrders[index].outputTimeout);
            } else {
                if (needWait) {
                    LOGD(mModule, "Reorder frame succeed, current use case %s "
                        "frame order %s -> %s, received %s.",
                        frameTypeName(type),
                        frameFormatName(gOrders[index].output1),
                        frameFormatName(gOrders[index].output2),
                        frameFormatName(gOrders[index].output1));
                }
            }
        }
    }

    return RETURNIGNORE(rc, NOT_REQUIRED);
}

void FrameReorder::reset(SemaphoreTimeout *sem)
{
    sem->signal();
    sem->signal();
    sem->wait();
    SECURE_DELETE(sem);
}

int32_t FrameReorder::reset(FrameType type)
{
    int32_t rc = NOT_FOUND;

    for (uint32_t i = 0; i < ARRAYSIZE(gOrders); i++) {
        if (type == gOrders[i].key) {
            if (NOTNULL(mOrderSems[i])) {
                reset(mOrderSems[i]);
                mOrderSems[i] = NULL;
                rc = NO_ERROR;
            }
            if (NOTNULL(mWaitSems[i])) {
                reset(mWaitSems[i]);
                mWaitSems[i] = NULL;
                rc = NO_ERROR;
            }
        }
    }

    return rc;
}

void FrameReorder::reset()
{
    for (int32_t i = 0; i < FRAME_TYPE_MAX_INVALID; i++) {
        reset(static_cast<FrameType>(i));
    }
}

int32_t FrameReorder::start(FrameType type)
{
    mStoped[type] = false;
    return reset(type);
}

int32_t FrameReorder::stop(FrameType type)
{
    mStoped[type] = true;
    return reset(type);
}

FrameReorder *FrameReorder::getInstance()
{
    if (ISNULL(gThis)) {
        pthread_mutex_lock(&gInsL);

        if (ISNULL(gThis)) {
            gThis = new FrameReorder();
            if (ISNULL(gThis)) {
                LOGE(mModule, "Failed to create "
                    "FrameReorder sigleton.");
            }
        }

        pthread_mutex_unlock(&gInsL);
    }

    if (NOTNULL(gThis)) {
        gCnt++;
    }

    return gThis;
}

uint32_t FrameReorder::removeInstance()
{
    ASSERT_LOG(mModule, gCnt > 0, "Instance not got.");
    ASSERT_LOG(mModule, NOTNULL(gThis), "Instance not created.");

    gCnt--;
    if (gCnt == 0 && NOTNULL(gThis)) {
        pthread_mutex_lock(&gInsL);

        delete gThis;
        gThis = NULL;

        pthread_mutex_unlock(&gInsL);
    }

    return gCnt;
}

FrameReorder::FrameReorder() :
    mOrderSems(NULL),
    mWaitSems(NULL)
{
    int32_t len = sizeof(Semaphore *) * ARRAYSIZE(gOrders);
    mOrderSems = (SemaphoreTimeout **)Malloc(len);
    mWaitSems = (SemaphoreTimeout **)Malloc(len);
    ASSERT_LOG(mModule, NOTNULL(mOrderSems) && NOTNULL(mWaitSems),
        "Failed to alloc %d bytes for semaphores.", len * 2);

    for (uint32_t i = 0; i < ARRAYSIZE(gOrders); i++) {
        mOrderSems[i] = mWaitSems[i] = NULL;
    }

    for (uint32_t i = 0; i < ARRAYSIZE(mStoped); i++) {
        mStoped[i] = false;
    }
}

FrameReorder::~FrameReorder()
{
    for (uint32_t i = 0; i < ARRAYSIZE(mStoped); i++) {
        stop(static_cast<FrameType>(i));
    }

    for (uint32_t i = 0; i < ARRAYSIZE(gOrders); i++) {
        if (NOTNULL(mOrderSems[i])) {
            reset(mOrderSems[i]);
            mOrderSems[i] = NULL;
        }
        if (NOTNULL(mWaitSems[i])) {
            reset(mWaitSems[i]);
            mWaitSems[i] = NULL;
        }
    }

    SECURE_FREE(mOrderSems);
    SECURE_FREE(mWaitSems);
}

const ModuleType FrameReorder::mModule = MODULE_UTILS;

uint32_t FrameReorder::gCnt = 0;

FrameReorder *FrameReorder::gThis = NULL;

pthread_mutex_t FrameReorder::gInsL = PTHREAD_MUTEX_INITIALIZER;

};

