#include <stdarg.h>

#include "JpegExif.h"
#include "AlgorithmHelper.h"
#include "PfLogger.h"
#include "MemMgmt.h"
#include "exif-data.h"

namespace pandora {

#define AVERAGE_PERFORMANCE_COUNT 20
#define DEFAULT_JPEG_QUAL         100
#define WAIT_TIMEOUT_MS           7000

INIT_ALGORITHM_TRAITS_FUNC(JpegExif);

JpegExif::JpegExif(uint32_t argNum, va_list va) :
    mInited(false),
    mModule(MODULE_ALGORITHM),
    mExifBuf(NULL),
    mExifSize(0),
    mBufSize(0),
    mSem(WAIT_TIMEOUT_MS)
{
    for (uint32_t i = 0; i < argNum; i++) {
        LOGD(mModule, "va list arg %d value %d", i, va_arg(va, int32_t));
    }
}

JpegExif::~JpegExif()
{
    if (mInited) {
        deinit();
    }
}

int32_t JpegExif::init()
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

int32_t JpegExif::deinit()
{
    int32_t rc = NO_ERROR;

    if (!mInited) {
        rc = NOT_INITED;
    }

    if (SUCCEED(rc)) {
        if (NOTNULL(mExifBuf)) {
            SECURE_FREE(mExifBuf);
        }
    }

    mExifSize = 0;
    mBufSize = 0;

    return rc;
}

int32_t JpegExif::process(TaskTypeT & task, ResultTypeT & /*result*/)
{
    int32_t rc = NO_ERROR;
    uint8_t *tmpExifBuf = NULL;
    ExifData *exif = NULL;

    if (SUCCEED(rc)) {
        exif = exif_data_new_from_data((uint8_t *)task.data, task.size);
        if (ISNULL(exif)) {
            LOGE(mModule, "allocate new exif data failed");
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        exif_data_save_data(exif, &tmpExifBuf, &mExifSize);
        if (!mExifSize) {
            LOGE(mModule, "save exif failed");
            rc = INTERNAL_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        if (ISNULL(mExifBuf) || mExifSize > mBufSize) {
            if (NOTNULL(mExifBuf)) {
                SECURE_FREE(mExifBuf);
            }

            mExifBuf = (uint8_t *)Malloc(mExifSize);
            if (ISNULL(mExifBuf)) {
                LOGE(mModule, "allocate mExifBuf failed");
                rc = NO_MEMORY;
            } else {
               mBufSize = mExifSize;
            }
        }
    }

    if (SUCCEED(rc)) {
        if (NOTNULL(tmpExifBuf)) {
            memcpy(mExifBuf, tmpExifBuf, mExifSize);
        }
    }

    if (SUCCEED(rc)) {
        if (NOTNULL(exif)) {
            exif_data_unref(exif);
        }

        if (NOTNULL(tmpExifBuf)) {
            free(tmpExifBuf);
        }
    }

    if (SUCCEED(rc)) {
        mSem.signal();
    }

    return rc;
}

int32_t JpegExif::process(TaskTypeT &task, TaskTypeT &out, ResultTypeT & result)
{
    int32_t rc = process(task, result);
    if (SUCCEED(rc)) {
        if (ISNULL(out.data) || task.size > out.size) {
            LOGE(mModule, "Failed to copy result, "
                "NULL ptr or insufficient size, %p %d/%d",
                out.data, out.size, task.size);
            rc = PARAM_INVALID;
        }
    }

    if (SUCCEED(rc)) {
        void *data = out.data;
        out = task;
        memcpy(data, task.data, task.size);
        out.data = data;
    }

    return rc;

}

int32_t JpegExif::setParm(ParmTypeT &/*parm*/)
{
    // Not necessary to handle externally parms
    return NO_ERROR;
}

int32_t JpegExif::update(UpdateTypeT &/*update*/)
{
    // Don't care any updates from other algorithms
    return NO_ERROR;
}

int32_t JpegExif::queryCaps(AlgCaps &caps)
{
    caps.frameNeeded    = SINGLE_FRAME_ALGORITHM_INPUT;
    caps.workingBufCnt  = 0;
    caps.workingBufSize = 0;
    caps.types   = FRAME_TYPE_MASK_SNAPSHOT;
    caps.formats = FRAME_FORMAT_MASK_JPEG;

    return NO_ERROR;
}

int32_t JpegExif::queryStatus(StatusTypeT &/*status*/)
{
    // No status changed and not necessary to notify caller
    return NOT_SUPPORTED;
}

int32_t JpegExif::waitAndReadExif(uint8_t **exifData, uint32_t *exifSize)
{
    uint32_t rc = NO_ERROR;

    rc = mSem.wait();
    if (SUCCEED(rc))  {
        *exifData = mExifBuf;
        *exifSize = mExifSize;
    }

    return rc;
}

}
