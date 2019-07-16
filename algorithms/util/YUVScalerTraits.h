#ifndef _SCALE_YUV_TRAITS_H_
#define _SCALE_YUV_TRAITS_H_

#include "AlgorithmIntf.h"

namespace pandora {

USE_DEFAULT_TASK_TYPE(YUVScaler);

USE_DEFAULT_UPDATE_TYPE(YUVScaler);

USE_DEFAULT_REQUEST_TYPE(YUVScaler);

USE_DEFAULT_STATUS_TYPE(YUVScaler);

struct YUVScalerResultType :
    public ResultType {
public:
    int32_t w;
    int32_t h;
    size_t size;
    void *data;

public:
    void dump() override {
        LOGI(MODULE_ALGORITHM, "YUV scaler result: "
            "w %d h %d size %ld data %p", w, h, size, data);
    }
};

struct YUVScalerParmType :
    public ParmType {
public:
    int32_t outputW;
    int32_t outputH;
};


DEFINE_ALGORITHM_TRAITS(YUVScaler);


};

#endif

