#ifndef _PANDORA_PAL_HPP_
#define _PANDORA_PAL_HPP_

#include "Pal.h"
#include "PalImpl.h"

namespace pandora {

#define CHECK_PAL_IMPL() \
    ({ \
        int32_t rc = NO_ERROR; \
        if (ISNULL(mPalImpl)) { \
            LOGE(mModule, "Pal Impl not inited"); \
            rc = NOT_INITED; \
        } \
        rc; \
    })

template<typename T>
int32_t Pal::getParm(T &parm, SyncTypeE sync)
{
    int32_t rc = CHECK_PAL_IMPL();
    return SUCCEED(rc) ? mPalImpl->getParm(parm, sync) : rc;
}

template<typename T>
int32_t Pal::setParm(T &parm, SyncTypeE sync)
{
    int32_t rc = CHECK_PAL_IMPL();
    return SUCCEED(rc) ? mPalImpl->setParm(parm, sync) : rc;
}

template<typename T>
int32_t Pal::getConfig(T &parm, SyncTypeE sync)
{
    int32_t rc = CHECK_PAL_IMPL();
    return SUCCEED(rc) ? mPalImpl->getConfig(parm, sync) : rc;
}

template<typename T>
int32_t Pal::evtNotify(T &evt, SyncTypeE sync)
{
    int32_t rc = CHECK_PAL_IMPL();
    return SUCCEED(rc) ? mPalImpl->evtNotify(evt, sync) : rc;
}

template<typename T>
int32_t Pal::dataNotify(T &data, SyncTypeE sync)
{
    int32_t rc = CHECK_PAL_IMPL();
    return SUCCEED(rc) ? mPalImpl->dataNotify(data, sync) : rc;
}

};

#endif
