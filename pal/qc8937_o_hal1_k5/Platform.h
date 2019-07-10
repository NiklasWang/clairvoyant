#ifndef _PLATFORM_QC8937_O_HAL1_
#define _PLATFORM_QC8937_O_HAL1_

namespace pandora {

#define PLATFORM_QC8937 qc8937
#define BRANCH_O        o
#define HAL_VERSION_1   hal1

#define PLATFORM PLATFORM_QC8937
#define BRANCH   BRANCH_O
#define HALV     HAL_VERSION_1

#define MyType \
    PLATFORM##_##BRANCH##_##HALV

};

#endif
