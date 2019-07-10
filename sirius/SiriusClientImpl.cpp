#include "SiriusData.h"
#include "SiriusClientCore.h"
#include "PreviewClient.h"
#include "YuvPictureClient.h"
#include "BayerPictureClient.h"
#include "EventClient.h"
#include "DataClient.h"
#include "SiriusClientImpl.h"

using namespace sirius;

#define module MODULE_SIRIUS

namespace pandora {

int32_t SiriusClientImpl::update(PreviewDim &preview, PictureDim &picture)
{
    int32_t rc = NO_ERROR;

    Header header = {
        .previewW = preview.w,
        .previewH = preview.h,
        .previewStride   = 1,
        .previewScanline = 1,
        .pictureW = picture.w,
        .pictureH = picture.h,
        .pictureStride   = 1,
        .pictureScanline = 1,
        .bayerPictureW = 0,
        .bayerPictureH = 0,
        .bayerPattern  = BAYER_PATTERN_RGGB,
    };

    if (SUCCEED(rc)) {
        if (ISNULL(mCore)) {
            mCore = new SiriusClientCore();
            if (ISNULL(mCore)) {
                LOGE(module, "Failed to create client core");
                rc = NO_MEMORY;
            }
        }
    }

    if (SUCCEED(rc)) {
        rc = mCore->update(header);
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to update header, %d", rc);
        }
    }

    return rc;
}

int32_t SiriusClientImpl::onFrameReady(SiriusTask &frame)
{
    int32_t rc = NO_ERROR;

    switch (frame.type) {
        case FRAME_TYPE_PREVIEW: {
            rc = sendPreview(frame);
        } break;
        case FRAME_TYPE_SNAPSHOT: {
            if (frame.format == FRAME_FORMAT_YUV_420_NV21 ||
                frame.format == FRAME_FORMAT_YUV_420_NV12) {
                rc = sendSnapshot(frame);
            } else if (frame.format == FRAME_FORMAT_BAYER) {
                rc = sendBayer(frame);
            }
        } break;
        default: {
        }
    }

    return rc;
}

int32_t SiriusClientImpl::sendPreview(SiriusTask &frame)
{
    int32_t rc = NO_ERROR;

    if (ISNULL(mPreview)) {
        rc = enablePreview();
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to enable preview, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = mPreview->onPreviewReady(frame.w, frame.h,
            frame.stride, frame.scanline, frame.data, frame.ts);
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to send to preview client, %d", rc);
        }
    }

    return rc;
}

int32_t SiriusClientImpl::sendSnapshot(SiriusTask &frame)
{
    int32_t rc = NO_ERROR;

    if (ISNULL(mSnapshot)) {
        rc = enableSnapshot();
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to enable snapshot, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = mSnapshot->onYuvPictureReady(frame.w, frame.h,
            frame.stride, frame.scanline, frame.data, frame.ts);
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to send to snapshot client, %d", rc);
        }
    }

    return rc;
}

int32_t SiriusClientImpl::sendBayer(SiriusTask &frame)
{
    int32_t rc = NO_ERROR;

    if (ISNULL(mBayer)) {
        rc = enableBayer();
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to enable bayer picture, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = mBayer->onBayerPictureReady(frame.w, frame.h,
            frame.data, frame.ts, BAYER_PATTERN_RGGB);
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to send to bayer client, %d", rc);
        }
    }

    return rc;
}

int32_t SiriusClientImpl::sendEvent(int32_t evt, int32_t arg1, int32_t arg2)
{
    int32_t rc = NO_ERROR;

    if (ISNULL(mEvent)) {
        rc = enableEvent();
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to enable event, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = mEvent->onEventReady(evt, arg1, arg2);
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to send to event client, %d", rc);
        }
    }

    return rc;
}

int32_t SiriusClientImpl::sendData(int32_t type, void *data, int32_t size)
{
    int32_t rc = NO_ERROR;

    if (ISNULL(mData)) {
        rc = enableData();
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to enable data, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = mData->onDataReady(type, data, size);
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to send to data client, %d", rc);
        }
    }

    return rc;
}

int32_t SiriusClientImpl::enablePreview()
{
    int32_t rc = NO_ERROR;

    if (NOTNULL(mPreview)) {
        LOGE(module, "Already created.");
        rc = ALREADY_EXISTS;
    }

    if (SUCCEED(rc)) {
        mPreview = new PreviewClient();
        if (ISNULL(mPreview)) {
            LOGE(module, "Failed to create preview client");
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        rc = mPreview->construct();
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to construct preview client, %d", rc);
        }
    }

    if (!SUCCEED(rc)) {
        if (NOTNULL(mPreview)) {
            SECURE_DELETE(mPreview);
        }
    }

    return rc;
}

int32_t SiriusClientImpl::enableSnapshot()
{
    int32_t rc = NO_ERROR;

    if (NOTNULL(mSnapshot)) {
        LOGE(module, "Already created.");
        rc = ALREADY_EXISTS;
    }

    if (SUCCEED(rc)) {
        mSnapshot = new YuvPictureClient();
        if (ISNULL(mSnapshot)) {
            LOGE(module, "Failed to create snapshot client");
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        rc = mSnapshot->construct();
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to construct snapshot client, %d", rc);
        }
    }

    if (!SUCCEED(rc)) {
        if (NOTNULL(mSnapshot)) {
            SECURE_DELETE(mSnapshot);
        }
    }

    return rc;
}

int32_t SiriusClientImpl::enableBayer()
{
    int32_t rc = NO_ERROR;

    if (NOTNULL(mBayer)) {
        LOGE(module, "Already created.");
        rc = ALREADY_EXISTS;
    }

    if (SUCCEED(rc)) {
        mBayer = new BayerPictureClient();
        if (ISNULL(mBayer)) {
            LOGE(module, "Failed to create bayer picture client");
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        rc = mBayer->construct();
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to construct bayer picture client, %d", rc);
        }
    }

    if (!SUCCEED(rc)) {
        if (NOTNULL(mBayer)) {
            SECURE_DELETE(mBayer);
        }
    }

    return rc;
}

int32_t SiriusClientImpl::enableEvent()
{
    int32_t rc = NO_ERROR;

    if (NOTNULL(mEvent)) {
        LOGE(module, "Already created.");
        rc = ALREADY_EXISTS;
    }

    if (SUCCEED(rc)) {
        mEvent = new EventClient();
        if (ISNULL(mEvent)) {
            LOGE(module, "Failed to create event client");
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        rc = mEvent->construct();
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to construct event client, %d", rc);
        }
    }

    if (!SUCCEED(rc)) {
        if (NOTNULL(mEvent)) {
            SECURE_DELETE(mEvent);
        }
    }

    return rc;
}

int32_t SiriusClientImpl::enableData()
{
    int32_t rc = NO_ERROR;

    if (NOTNULL(mData)) {
        LOGE(module, "Already created.");
        rc = ALREADY_EXISTS;
    }

    if (SUCCEED(rc)) {
        mData = new DataClient();
        if (ISNULL(mData)) {
            LOGE(module, "Failed to create data client");
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        rc = mData->construct();
        if (!SUCCEED(rc)) {
            LOGE(module, "Failed to construct data client, %d", rc);
        }
    }

    if (!SUCCEED(rc)) {
        if (NOTNULL(mData)) {
            SECURE_DELETE(mData);
        }
    }

    return rc;
}

SiriusClientImpl::SiriusClientImpl() :
    mPreview(NULL),
    mSnapshot(NULL),
    mBayer(NULL),
    mEvent(NULL),
    mData(NULL),
    mCore(NULL)
{
}

SiriusClientImpl::~SiriusClientImpl()
{
    if (NOTNULL(mCore)) {
        SECURE_DELETE(mCore);
    }
    if (NOTNULL(mPreview)) {
        mPreview->destruct();
        SECURE_DELETE(mPreview);
    }
    if (NOTNULL(mSnapshot)) {
        mSnapshot->destruct();
        SECURE_DELETE(mSnapshot);
    }
    if (NOTNULL(mBayer)) {
        mBayer->destruct();
        SECURE_DELETE(mBayer);
    }
    if (NOTNULL(mEvent)) {
        mEvent->destruct();
        SECURE_DELETE(mEvent);
    }
    if (NOTNULL(mData)) {
        mData->destruct();
        SECURE_DELETE(mData);
    }
}

};

