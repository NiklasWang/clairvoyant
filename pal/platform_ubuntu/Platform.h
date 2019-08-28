#ifndef _PLATFORM_UBUNTU_
#define _PLATFORM_UBUNTU_

namespace pandora {

#define PLATFORM_UBUNTU ubuntu
#define BRANCH_O        o
#define HAL_VERSION_1   hal1

#define PLATFORM PLATFORM_UBUNTU
#define BRANCH   BRANCH_O
#define HALV     HAL_VERSION_1

#define MyType \
    PLATFORM##_##BRANCH##_##HALV

};

#endif
