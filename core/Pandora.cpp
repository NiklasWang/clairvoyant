#include "Pandora.h"
#include "PandoraImpl.h"
#include "PfLogger.h"

namespace pandora {

#define CHECK_VALID_IMPL() \
    ({ \
        int32_t __rc = NO_ERROR; \
        if (ISNULL(mImpl)) { \
            __rc = NOT_INITED; \
            LOGE(MODULE_PANDORA, "Pandora impl not created"); \
        } \
        __rc; \
    })

int32_t Pandora::startPreview()
{
    int32_t rc = CHECK_VALID_IMPL();
    return SUCCEED(rc) ? mImpl->startPreview() : rc;
}

int32_t Pandora::stopPreview()
{
    int32_t rc = CHECK_VALID_IMPL();
    return SUCCEED(rc) ? mImpl->stopPreview() : rc;
}

int32_t Pandora::takePicture()
{
    int32_t rc = CHECK_VALID_IMPL();
    return SUCCEED(rc) ? mImpl->takePicture() : rc;
}

int32_t Pandora::pictureTaken()
{
    int32_t rc = CHECK_VALID_IMPL();
    return SUCCEED(rc) ? mImpl->pictureTaken() : rc;
}

int32_t Pandora::startRecording()
{
    int32_t rc = CHECK_VALID_IMPL();
    return SUCCEED(rc) ? mImpl->startRecording() : rc;
}

int32_t Pandora::stopRecording()
{
    int32_t rc = CHECK_VALID_IMPL();
    return SUCCEED(rc) ? mImpl->stopRecording() : rc;
}

int32_t Pandora::onMetadataAvailable(void *metadata)
{
    int32_t rc = CHECK_VALID_IMPL();
    return SUCCEED(rc) ? mImpl->onMetadataAvailable(metadata) : rc;
}

int32_t Pandora::onCommandAvailable(
    int32_t command, int32_t arg1, int32_t arg2)
{
    int32_t rc = CHECK_VALID_IMPL();
    return SUCCEED(rc) ? mImpl->onCommandAvailable(command, arg1, arg2) : rc;
}

int32_t Pandora::onParameterAvailable(const void *parm)
{
    int32_t rc = CHECK_VALID_IMPL();
    return SUCCEED(rc) ? mImpl->onParameterAvailable(parm) : rc;
}

int32_t Pandora::onFrameReady(FrameInfo &frame)
{
    int32_t rc = CHECK_VALID_IMPL();
    return SUCCEED(rc) ? mImpl->onFrameReady(frame) : rc;
}

Pandora::Pandora(PlatformOpsIntf *platform)
{
    mImpl = new PandoraImpl(platform);
    if (ISNULL(mImpl)) {
        LOGE(MODULE_PANDORA, "Failed to create pandora impl");
    }
}

Pandora::~Pandora()
{
    PandoraInterface *impl = mImpl;
    mImpl = NULL;
    SECURE_DELETE(impl);

    // Clear performance checker
    if (NOTNULL(PfLogger::getInstance())) {
        PfLogger::getInstance()->clear();
        delete PfLogger::getInstance();
    }

    // Dump memory usage in case of leakage
    Dump();
}

};

