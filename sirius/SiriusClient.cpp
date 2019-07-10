#include "Common.h"
#include "SiriusClient.h"
#include "SiriusClientImpl.h"

namespace pandora {

#define CHECK_VALID_IMPL() \
    ({ \
        int32_t __rc = NO_ERROR; \
        if (ISNULL(mImpl)) { \
            LOGD(MODULE_SIRIUS, "Sirius client impl not created"); \
            __rc = NOT_INITED; \
        } \
        __rc; \
    })

#define CONSTRUCT_IMPL() \
    ({ \
        int32_t __rc = NO_ERROR; \
        if (ISNULL(mImpl)) { \
            mImpl = new SiriusClientImpl(); \
            if (ISNULL(mImpl)) { \
                LOGE(MODULE_SIRIUS, "Failed to create Sirius client impl"); \
                __rc = NOT_INITED; \
            } \
        } \
        __rc; \
    })


#define CONSTRUCT_IMPL_ONCE() \
    ({ \
        int32_t __rc = CHECK_VALID_IMPL(); \
        if (__rc == NOT_INITED) { \
            __rc = CONSTRUCT_IMPL(); \
        } \
        __rc; \
    })

#ifdef ENABLE_SIRIUS

#include "SiriusClientImpl.h"

int32_t SiriusClient::update(PreviewDim &preview, PictureDim &picture)
{
    int32_t rc = CONSTRUCT_IMPL_ONCE();
    return SUCCEED(rc) ? mImpl->update(preview, picture) : rc;
}

int32_t SiriusClient::onFrameReady(SiriusTask &frame)
{
    int32_t rc = CONSTRUCT_IMPL_ONCE();
    return SUCCEED(rc) ? mImpl->onFrameReady(frame) : rc;
}

int32_t SiriusClient::sendEvent(int32_t evt, int32_t arg1, int32_t arg2)
{
    int32_t rc = CONSTRUCT_IMPL_ONCE();
    return SUCCEED(rc) ? mImpl->sendEvent(evt, arg1, arg2) : rc;
}

int32_t SiriusClient::sendData(int32_t type, void *data, int32_t size)
{
    int32_t rc = CONSTRUCT_IMPL_ONCE();
    return SUCCEED(rc) ? mImpl->sendData(type, data, size) : rc;
}

#else

int32_t SiriusClient::update(PreviewDim & /*preview*/, PictureDim & /*picture*/)
{
    return NO_ERROR;
}

int32_t SiriusClient::onFrameReady(SiriusTask & /*frame*/)
{
    return NO_ERROR;
}

int32_t SiriusClient::sendEvent(int32_t /*evt*/, int32_t /*arg1*/, int32_t /*arg2*/)
{
    return NO_ERROR;
}

int32_t SiriusClient::sendData(int32_t /*type*/, void * /*data*/, int32_t /*size*/)
{
    return NO_ERROR;
}

#endif

SiriusClient::SiriusClient() :
    mImpl(NULL)
{
}

SiriusClient::~SiriusClient()
{
    if (NOTNULL(mImpl)) {
        SECURE_DELETE(mImpl);
    }
}

};

