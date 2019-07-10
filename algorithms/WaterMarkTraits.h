#ifndef __WATER_MARK_TRAITS_H_
#define __WATER_MARK_TRAITS_H_

#include "AlgorithmIntf.h"
#include "PandoraParameters.h"

namespace pandora {

USE_DEFAULT_TASK_TYPE(WaterMark);

USE_DEFAULT_RESULT_TYPE(WaterMark);

USE_DEFAULT_UPDATE_TYPE(WaterMark);

USE_DEFAULT_REQUEST_TYPE(WaterMark);

USE_DEFAULT_STATUS_TYPE(WaterMark);


struct  WaterMarkParmType :
    public ParmType {
public:
    RotationAngle rotation;
    WaterMarkInfo info;
};

DEFINE_ALGORITHM_TRAITS(WaterMark);


};

#endif


