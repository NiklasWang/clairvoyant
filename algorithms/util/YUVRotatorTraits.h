#ifndef _ROTATE_YUV_TRAITS_H_
#define _ROTATE_YUV_TRAITS_H_

#include "AlgorithmIntf.h"

namespace pandora {

USE_DEFAULT_TASK_TYPE(YUVRotator);

USE_DEFAULT_UPDATE_TYPE(YUVRotator);

USE_DEFAULT_REQUEST_TYPE(YUVRotator);

USE_DEFAULT_STATUS_TYPE(YUVRotator);

struct YUVRotatorResultType :
    public ResultType {
public:
    int32_t w;
    int32_t h;
    size_t size;
    void *data;

public:
    void dump() override {
        LOGI(MODULE_ALGORITHM, "YUV rotator result: "
            "w %d h %d size %ld data %p", w, h, size, data);
    }
};

enum RotateOption {
    YUV_ROTATOR_0,
    YUV_ROTATOR_90,
    YUV_ROTATOR_180,
    YUV_ROTATOR_270,
    YUV_ROTATOR_INVALID_MAX,
};

struct YUVRotatorParmType :
    public ParmType {
public:
    RotateOption option;
};


DEFINE_ALGORITHM_TRAITS(YUVRotator);


};

#endif

