#include "PandoraSingleton.h"
#include "Common.h"

namespace pandora {

uint32_t PandoraSingleton::gCnt = 0;

PandoraSingleton *PandoraSingleton::gThis = NULL;

pthread_mutex_t PandoraSingleton::gInsL = PTHREAD_MUTEX_INITIALIZER;

PandoraSingleton *PandoraSingleton::getInstance(PlatformOpsIntf *platform)
{
    if (ISNULL(gThis)) {
        pthread_mutex_lock(&gInsL);

        if (ISNULL(gThis)) {
            gThis = new PandoraSingleton(platform);
            if (ISNULL(gThis)) {
                LOGE(MODULE_PANDORA, "Failed to create Pandora sigleton.");
            }
        }

        pthread_mutex_unlock(&gInsL);
    }

    if (NOTNULL(gThis)) {
        gCnt++;
    }

    return gThis;
}

uint32_t PandoraSingleton::removeInstance()
{
    ASSERT_LOG(MODULE_PANDORA, gCnt > 0, "Instance not got.");
    ASSERT_LOG(MODULE_PANDORA, NOTNULL(gThis), "Instance not created.");

    gCnt--;
    if (gCnt == 0 && NOTNULL(gThis)) {
        pthread_mutex_lock(&gInsL);

        delete gThis;
        gThis = NULL;

        pthread_mutex_unlock(&gInsL);
    }

    return gCnt;
}

PandoraSingleton::PandoraSingleton(PlatformOpsIntf *platform) :
    Pandora(platform)
{}

PandoraSingleton::~PandoraSingleton()
{
}

};

