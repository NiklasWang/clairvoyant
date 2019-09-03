#include "Pal.h"
#include "PalImpl.h"
#include "PlatformOpsIntf.h"

namespace pandora {

Pal::Pal(PlatformOpsIntf *platform) :
    mConstructed(false),
    mModule(MODULE_PAL),
    mPlatform(platform),
    mPalImpl(NULL)
{
    if (NOTNULL(mPlatform) && mPlatform->isInited()) {
        LOGI(mModule, "> Binding with platform hal ver: %s",
            mPlatform->whoamI());
    } else {
        LOGE(mModule, "Fatal: platform hal not inited");
    }
}

Pal::~Pal()
{
    if (mConstructed) {
        destruct();
    }
}

int32_t Pal::construct()
{
    int32_t rc = NO_ERROR;

    if (mConstructed) {
        rc = ALREADY_EXISTS;
    }

    if (SUCCEED(rc)) {
        mPalImpl = new PalImpl(mPlatform);
        if (ISNULL(mPalImpl)) {
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        rc = mPalImpl->construct();
        if (!SUCCEED(rc)) {
            delete mPalImpl;
            mPalImpl = NULL;
            LOGE(mModule, "Failed to construct pal impl");
        }
    }

    if (SUCCEED(rc)) {
        mConstructed = true;
        LOGD(mModule, "Pal constructed");
    }

    return RETURNIGNORE(rc, ALREADY_EXISTS);
}

int32_t Pal::destruct()
{
    int32_t rc = NO_ERROR;

    if (!mConstructed) {
        rc = NOT_INITED;
    } else {
        mConstructed = false;
    }

    if (NOTNULL(mPalImpl)) {
        rc = mPalImpl->destruct();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to destruct pal impl");
        } else {
            SECURE_DELETE(mPalImpl);
        }
    }

    if (!SUCCEED(rc)) {
        mConstructed = true;
        LOGE(mModule, "Failed to destruct pal");
    } else {
        LOGD(mModule, "Pal destructed");
    }

    return RETURNIGNORE(rc, NOT_INITED);
}

#define CHECK_PAL_IMPL() \
    ({ \
        int32_t rc = NO_ERROR; \
        if (ISNULL(mPalImpl)) { \
            LOGE(mModule, "Pal Impl not inited"); \
            rc = NOT_INITED; \
        } \
        rc; \
    })

int32_t Pal::onMetadataAvailable(const void *metadata)
{
    int32_t rc = CHECK_PAL_IMPL();
    return SUCCEED(rc) ? mPalImpl->onMetadataAvailable(metadata) : rc;
}

int32_t Pal::onParameterAvailable(const void *parm)
{
    int32_t rc = CHECK_PAL_IMPL();
    return SUCCEED(rc) ? mPalImpl->onParameterAvailable(parm) : rc;
}

int32_t Pal::onCommandAvailable(int32_t cmd, int32_t arg1, int32_t arg2)
{
    int32_t rc = CHECK_PAL_IMPL();
    return SUCCEED(rc) ? mPalImpl->onCommandAvailable(cmd, arg1, arg2) : rc;
}

};

