#ifndef _PLATFORM_QC8996_N_HAL1_
#define _PLATFORM_QC8996_N_HAL1_

namespace pandora {

#define PLATFORM_QC8996 qc8996
#define BRANCH_N        n
#define HAL_VERSION_1   hal1

#define PLATFORM PLATFORM_QC8996
#define BRANCH   BRANCH_N
#define HALV     HAL_VERSION_1

#define MyType \
    PLATFORM##_##BRANCH##_##HALV

};

#endif
