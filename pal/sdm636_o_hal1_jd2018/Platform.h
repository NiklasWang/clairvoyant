#ifndef _PLATFORM_SDM636_O_HAL1_
#define _PLATFORM_SDM636_O_HAL1_

namespace pandora {

#define PLATFORM_SDM636 sdm636
#define BRANCH_O        o
#define HAL_VERSION_1   hal1

#define PLATFORM PLATFORM_SDM636
#define BRANCH   BRANCH_O
#define HALV     HAL_VERSION_1

#define MyType \
    PLATFORM##_##BRANCH##_##HALV

};

#endif
