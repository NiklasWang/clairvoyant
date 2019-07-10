#ifndef __PANDORA_SINGLETON_H_
#define __PANDORA_SINGLETON_H_

#include <pthread.h>

#include "Pandora.h"

namespace pandora {

class PandoraSingleton :
    public Pandora {
public:
    static PandoraSingleton *getInstance(PlatformOpsIntf *platform);
    static uint32_t removeInstance();

private:
    PandoraSingleton(PlatformOpsIntf *platform);
    virtual ~PandoraSingleton();
    PandoraSingleton(const PandoraSingleton &rhs);
    PandoraSingleton &operator =(const PandoraSingleton &rhs);

private:
    static uint32_t gCnt;
    static pthread_mutex_t   gInsL;
    static PandoraSingleton *gThis;
};

};

#endif
